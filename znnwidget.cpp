#include "znnwidget.h"
unsigned int VBO,VAO,EBO;
znnwidget::znnwidget(QWidget *parent) :
    QOpenGLWidget(parent),
    Data_VBO(QOpenGLBuffer::VertexBuffer),      // 指定为vbo
    Surface_EBO(QOpenGLBuffer::IndexBuffer),    // 指定为ebo
    Slice_EBO(QOpenGLBuffer::IndexBuffer)       // 指定为ebo
{
    setFocusPolicy(Qt::StrongFocus);
    connect(&timer_1,SIGNAL(timeout()),this,SLOT(on_timeout()));
}

void znnwidget::drawshape(znnwidget::Shape shape)
{
    m_shape = shape;
    update();
}

void znnwidget::recover()
{
    offsetX = 0;
    offsetY = 0;
    angleX = 0;
    angleY = 0;
    angleZ = 0;

}
unsigned int indices[] = {
    // 前面
    0, 1, 1, 2, 2, 3, 3, 0,
    // 后面
    4, 5, 5, 6, 6, 7, 7, 4,
    // 连接前后
    0, 4, 1, 5, 2, 6, 3, 7
};
//下面的函数用来计算坐标系的投影位置
QPoint znnwidget::projectToScreen(const QVector3D &worldPos, const QMatrix4x4 &model, const QMatrix4x4 &view, const QMatrix4x4 &proj)
{
    QVector4D clipSpacePos = proj * view * model * QVector4D(worldPos, 1.0f);
    if (clipSpacePos.w() == 0.0f) return QPoint(-1000, -1000); // 防止除以0
    QVector3D ndc = clipSpacePos.toVector3DAffine(); // 归一化设备坐标
    int x = int((ndc.x() * 0.5f + 0.5f) * width());
    int y = int((1.0f - (ndc.y() * 0.5f + 0.5f)) * height()); // 注意Y轴反过来
    return QPoint(x, y);
}


void znnwidget::initializeGL()
{
    // 创建 VAO 和 VBO 和 EBO
    initializeOpenGLFunctions();
    vertices = {
        // 前面
        {{-Axis_x, -Axis_y,  Axis_z}, {1.0f, 1.0f, 1.0f}}, // 0
        {{ Axis_x, -Axis_y,  Axis_z}, {1.0f, 1.0f, 1.0f}}, // 1
        {{ Axis_x,  Axis_y,  Axis_z}, {1.0f, 1.0f, 1.0f}}, // 2
        {{-Axis_x,  Axis_y,  Axis_z}, {1.0f, 1.0f, 1.0f}}, // 3

        // 后面
        {{-Axis_x, -Axis_y, -Axis_z}, {1.0f, 1.0f, 1.0f}}, // 4
        {{ Axis_x, -Axis_y, -Axis_z}, {1.0f, 1.0f, 1.0f}}, // 5
        {{ Axis_x,  Axis_y, -Axis_z}, {1.0f, 1.0f, 1.0f}}, // 6
        {{-Axis_x,  Axis_y, -Axis_z}, {1.0f, 1.0f, 1.0f}}, // 7
    };

    glEnable(GL_DEPTH_TEST);
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders.vert");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    shaderProgram.link(); // 链接
    shaderProgram.bind();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1,&EBO);
    // 绑定 VAO（开始记录状态）
    glBindVertexArray(VAO);
    // 绑定 VBO 并上传数据
    // 绑定 EBO 并上传数据
    // EBO 必须在 VAO绑定的时候 绑定
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);
    // 设置顶点属性规则（记录到 VAO 中）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));
    glEnableVertexAttribArray(1);
    // 解绑 VBO（可选，但推荐）
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑 VAO（防止意外修改）
    glBindVertexArray(0);

    // 初始化Data_VBO，这一步先不关联VAO
    // 先上传空数据给GPU，在读入文件数据时重新 allocate
    Data_VBO.create();
    Data_VBO.bind();
    Data_VBO.setUsagePattern(QOpenGLBuffer::StaticDraw); // 静态
    Data_VBO.allocate(nullptr, 0); // 分配空数据
    Data_VBO.release();

    // 关联 Surface_VAO Data_VBO Surface_EBO
    // Surface_EBO 在读入文件数据时重新 allocate
    Surface_VAO.create();
    Surface_VAO.bind();

    Data_VBO.bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));
    glEnableVertexAttribArray(1);

    Surface_EBO.create();
    Surface_EBO.bind();
    Surface_EBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    Surface_EBO.allocate(nullptr, 0);
    Data_VBO.release();
    Surface_VAO.release();

    // 关联 Slice_VAO Data_VBO Slice_EBO
    // Slive_EBO 根据指定的平面重新确定
    Slice_VAO.create();
    Slice_VAO.bind();

    Data_VBO.bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));
    glEnableVertexAttribArray(1);

    Slice_EBO.create();
    Slice_EBO.bind();
    Slice_EBO.setUsagePattern(QOpenGLBuffer::DynamicDraw); // 动态
    Slice_EBO.allocate(nullptr, 0);
    Data_VBO.release();
    Slice_VAO.release();
}

void znnwidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h)
}

void znnwidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 5.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram.bind();
    QMatrix4x4 model;
    model.scale(scalen);  // 放大
    model.translate(offsetX, offsetY, 0.0f);
    model.rotate(angleX, 1.0f, 0.0f, 0.0f);
    model.rotate(angleY, 0.0f, 1.0f, 0.0f);
    model.rotate(angleZ, 0.0f, 0.0f, 1.0f); // 多轴旋转看起来更立体
    shaderProgram.setUniformValue("model", model);
    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -3.0f); // 视角向后拉
    QMatrix4x4 projection;
    projection.perspective(45.0f, float(width())/height(), 0.1f, 100.0f);

    shaderProgram.setUniformValue("view", view);
    shaderProgram.setUniformValue("projection", projection);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);
    switch (m_shape) {
    case Rect:
        timer_1.start(10);

        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

        break;
    default:
        break;
    }
    glBindVertexArray(0);
    glUseProgram(0);
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));

    for (const auto& v : vertices) {
        QPoint screenPos = projectToScreen(v.position, model, view, projection);
        painter.drawText(screenPos, QString("(%1,%2,%3)")
                         .arg(v.position.x())
                         .arg(v.position.y())
                         .arg(v.position.z()));
    }
    painter.end();
}

// 调用完此函数后，在 update 中先绑定 Slice_VAO, 再 glDrawElements(GL_TRIANGLES, Slice_idx.size(), GL_UNSIGNED_INT, 0);
void znnwidget::DrawPlane(unsigned st_x, unsigned st_y, unsigned st_z, unsigned en_x, unsigned en_y, unsigned en_z)
{
    // (x, y, z) -> ((num_y * num_z) * x + num_z * y + z)
    unsigned d_fst = 0, d_sec = 0;
    if (st_x == en_x) {
        en_x++;
        d_fst = num_z, d_sec = 1u;
    }
    else if (st_y == en_y) {
        en_y++;
        d_fst = num_y * num_z, d_sec = 1u;
    }
    else if (st_z == en_z) {
        en_z++;
        d_fst = num_y * num_z, d_sec = num_z;
    }
    else {
        // 说明传入的不是一个平面

        return;
    }

    Slice_idx.clear();
    for (unsigned x = st_x; x < en_x; x++) {
        for (unsigned y = st_y; y < en_y; y++) {
            for (unsigned z = st_z; z < en_z; z++) {
                unsigned base = (num_y * num_z) * x + num_z * y + z;
                // 传入右上角和左下角两个三角形顶点在 Data_VBO 中的位置
                Slice_idx << base << base + d_sec << base + d_fst + d_sec;
                Slice_idx << base << base + d_fst << base + d_fst + d_sec;
            }
        }
    }

    Slice_VAO.bind();
    Slice_EBO.bind();
    Slice_EBO.allocate(Slice_idx.constData(), sizeof(unsigned) * (unsigned)(Slice_idx.size()));
    Slice_VAO.release();

    update();
}

void znnwidget::on_timeout()
{
    makeCurrent();
    if(is_xc) angleZ += 2.0f;
    if (angleX >= 360.0f) angleZ -= 360.0f;
    doneCurrent();
    update();
}
void znnwidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:
        offsetY += 0.05f;
        break;
    case Qt::Key_S:
        offsetY -= 0.05f;
        break;
    case Qt::Key_A:
        offsetX -= 0.05f;
        break;
    case Qt::Key_D:
        offsetX += 0.05f;
        break;
    default:
        break;
    }
    update(); // 重新绘制
}
znnwidget::~znnwidget()
{
    if(!isValid()) return;
    makeCurrent();
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&EBO);
    doneCurrent();
}
