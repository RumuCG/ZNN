#include "znnwidget.h"
#include "vertexdata.h"
znnwidget::znnwidget(QWidget *parent) :
    QOpenGLWidget(parent),
    Axis_VBO(QOpenGLBuffer::VertexBuffer),      // 指定为vbo
    Axis_EBO(QOpenGLBuffer::IndexBuffer),       // 指定为ebo
    Data_VBO(QOpenGLBuffer::VertexBuffer),      // 指定为vbo
    Surface_EBO(QOpenGLBuffer::IndexBuffer),    // 指定为ebo
    Slice_EBO(QOpenGLBuffer::IndexBuffer)       // 指定为ebo
{
    setFocusPolicy(Qt::StrongFocus);
    connect(&timer_1,SIGNAL(timeout()),this,SLOT(on_timeout()));
    gshzb();
}

void znnwidget::setShape(znnwidget::Shape shape)
{
    m_shape = shape;
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

    QVector4D eyePos = view * model * QVector4D(worldPos, 1.0f);
    if (eyePos.z() >= 0.0f)  // 在相机后面，忽略
        return QPoint(-1000, -1000);
    QVector4D clipSpacePos = proj * eyePos;
    if (clipSpacePos.w() == 0.0f) return QPoint(-1000, -1000); // 防止除以0
    QVector3D ndc = clipSpacePos.toVector3DAffine(); // 归一化设备坐标
    if (ndc.x() < -1 || ndc.x() > 1 || ndc.y() < -1 || ndc.y() > 1 || ndc.z() < -1 || ndc.z() > 1)
        return QPoint(-1000, -1000); // 超出视锥，忽略
    int x = int((ndc.x() * 0.5f + 0.5f) * width());
    int y = int((1.0f - (ndc.y() * 0.5f + 0.5f)) * height()); // Y轴反转
    return QPoint(x, y);
}

float znnwidget::intoout(float x,int p)
{
    if(p == 0) return (x - min_[p]) / (max_[p] - min_[p]) * 2 * Axis_x - Axis_x;
    else if(p == 1) return (x - min_[p]) / (max_[p] - min_[p]) * 2 * Axis_y - Axis_y;
    else return (x - min_[p]) / (max_[p] - min_[p]) * 2 * Axis_z - Axis_z;
}
float znnwidget::outtoin(float x, int p)
{
    if(p == 0) return ((x + Axis_x) / (2 * Axis_x)) * (max_[p] - min_[p]) + min_[p];
    else if(p == 1) return ((x + Axis_y) / (2 * Axis_y)) * (max_[p] - min_[p]) + min_[p];
    else return ((x + Axis_z) / (2 * Axis_z)) * (max_[p] - min_[p]) + min_[p];
}

// 4 * 4 * 4 的矩阵
//std::vector<VertexData> modelData {
//    {{-0.5f, -0.25f, -0.25f}, {0.1f, 0.3f, 0.9f}},
//    {{-0.5f, -0.25f, -0.0833333f}, {0.4f, 0.7f, 0.2f}},
//    {{-0.5f, -0.25f, 0.0833333f}, {0.8f, 0.5f, 0.6f}},
//    {{-0.5f, -0.25f, 0.25f}, {0.2f, 0.1f, 0.4f}},
//    {{-0.5f, -0.0833333f, -0.25f}, {0.9f, 0.0f, 0.3f}},
//    {{-0.5f, -0.0833333f, -0.0833333f}, {0.5f, 0.5f, 0.5f}},
//    {{-0.5f, -0.0833333f, 0.0833333f}, {0.7f, 0.2f, 0.8f}},
//    {{-0.5f, -0.0833333f, 0.25f}, {0.3f, 0.6f, 0.1f}},
//    {{-0.5f, 0.0833333f, -0.25f}, {0.0f, 0.4f, 0.7f}},
//    {{-0.5f, 0.0833333f, -0.0833333f}, {0.6f, 0.9f, 0.2f}},
//    {{-0.5f, 0.0833333f, 0.0833333f}, {0.1f, 0.5f, 0.3f}},
//    {{-0.5f, 0.0833333f, 0.25f}, {0.8f, 0.1f, 0.6f}},
//    {{-0.5f, 0.25f, -0.25f}, {0.4f, 0.7f, 0.0f}},
//    {{-0.5f, 0.25f, -0.0833333f}, {0.3f, 0.2f, 0.5f}},
//    {{-0.5f, 0.25f, 0.0833333f}, {0.9f, 0.8f, 0.4f}},
//    {{-0.5f, 0.25f, 0.25f}, {0.5f, 0.3f, 0.7f}},
//    {{-0.166667f, -0.25f, -0.25f}, {0.2f, 0.6f, 0.1f}},
//    {{-0.166667f, -0.25f, -0.0833333f}, {0.7f, 0.4f, 0.9f}},
//    {{-0.166667f, -0.25f, 0.0833333f}, {0.0f, 0.8f, 0.5f}},
//    {{-0.166667f, -0.25f, 0.25f}, {0.3f, 0.1f, 0.6f}},
//    {{-0.166667f, -0.0833333f, -0.25f}, {0.5f, 0.7f, 0.2f}},
//    {{-0.166667f, -0.0833333f, -0.0833333f}, {0.8f, 0.3f, 0.4f}},
//    {{-0.166667f, -0.0833333f, 0.0833333f}, {0.1f, 0.9f, 0.0f}},
//    {{-0.166667f, -0.0833333f, 0.25f}, {0.6f, 0.5f, 0.7f}},
//    {{-0.166667f, 0.0833333f, -0.25f}, {0.4f, 0.2f, 0.8f}},
//    {{-0.166667f, 0.0833333f, -0.0833333f}, {0.9f, 0.6f, 0.1f}},
//    {{-0.166667f, 0.0833333f, 0.0833333f}, {0.2f, 0.7f, 0.3f}},
//    {{-0.166667f, 0.0833333f, 0.25f}, {0.5f, 0.0f, 0.9f}},
//    {{-0.166667f, 0.25f, -0.25f}, {0.7f, 0.1f, 0.4f}},
//    {{-0.166667f, 0.25f, -0.0833333f}, {0.3f, 0.8f, 0.6f}},
//    {{-0.166667f, 0.25f, 0.0833333f}, {0.6f, 0.4f, 0.2f}},
//    {{-0.166667f, 0.25f, 0.25f}, {0.1f, 0.5f, 0.7f}},
//    {{0.166667f, -0.25f, -0.25f}, {0.8f, 0.3f, 0.5f}},
//    {{0.166667f, -0.25f, -0.0833333f}, {0.4f, 0.9f, 0.1f}},
//    {{0.166667f, -0.25f, 0.0833333f}, {0.5f, 0.2f, 0.6f}},
//    {{0.166667f, -0.25f, 0.25f}, {0.9f, 0.7f, 0.0f}},
//    {{0.166667f, -0.0833333f, -0.25f}, {0.1f, 0.4f, 0.8f}},
//    {{0.166667f, -0.0833333f, -0.0833333f}, {0.6f, 0.3f, 0.5f}},
//    {{0.166667f, -0.0833333f, 0.0833333f}, {0.2f, 0.8f, 0.7f}},
//    {{0.166667f, -0.0833333f, 0.25f}, {0.7f, 0.5f, 0.3f}},
//    {{0.166667f, 0.0833333f, -0.25f}, {0.3f, 0.6f, 0.9f}},
//    {{0.166667f, 0.0833333f, -0.0833333f}, {0.5f, 0.1f, 0.4f}},
//    {{0.166667f, 0.0833333f, 0.0833333f}, {0.8f, 0.7f, 0.2f}},
//    {{0.166667f, 0.0833333f, 0.25f}, {0.0f, 0.5f, 0.6f}},
//    {{0.166667f, 0.25f, -0.25f}, {0.4f, 0.3f, 0.1f}},
//    {{0.166667f, 0.25f, -0.0833333f}, {0.9f, 0.8f, 0.5f}},
//    {{0.166667f, 0.25f, 0.0833333f}, {0.2f, 0.6f, 0.7f}},
//    {{0.166667f, 0.25f, 0.25f}, {0.7f, 0.4f, 0.0f}},
//    {{0.5f, -0.25f, -0.25f}, {0.6f, 0.1f, 0.5f}},
//    {{0.5f, -0.25f, -0.0833333f}, {0.3f, 0.7f, 0.8f}},
//    {{0.5f, -0.25f, 0.0833333f}, {0.5f, 0.9f, 0.2f}},
//    {{0.5f, -0.25f, 0.25f}, {0.8f, 0.0f, 0.4f}},
//    {{0.5f, -0.0833333f, -0.25f}, {0.1f, 0.5f, 0.6f}},
//    {{0.5f, -0.0833333f, -0.0833333f}, {0.4f, 0.2f, 0.9f}},
//    {{0.5f, -0.0833333f, 0.0833333f}, {0.7f, 0.3f, 0.1f}},
//    {{0.5f, -0.0833333f, 0.25f}, {0.0f, 0.8f, 0.5f}},
//    {{0.5f, 0.0833333f, -0.25f}, {0.9f, 0.6f, 0.3f}},
//    {{0.5f, 0.0833333f, -0.0833333f}, {0.2f, 0.4f, 0.7f}},
//    {{0.5f, 0.0833333f, 0.0833333f}, {0.5f, 0.1f, 0.8f}},
//    {{0.5f, 0.0833333f, 0.25f}, {0.6f, 0.5f, 0.0f}},
//    {{0.5f, 0.25f, -0.25f}, {0.3f, 0.9f, 0.4f}},
//    {{0.5f, 0.25f, -0.0833333f}, {0.8f, 0.2f, 0.5f}},
//    {{0.5f, 0.25f, 0.0833333f}, {0.1f, 0.7f, 0.6f}},
//    {{0.5f, 0.25f, 0.25f}, {0.4f, 0.0f, 0.9f}},
//};

void znnwidget::initializeGL(){
    // 创建 VAO 和 VBO 和 EBO
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders.vert");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    shaderProgram.link(); // 链接
    shaderProgram.bind();

    Axis_VAO.create();
    Axis_VBO.create();
    Axis_EBO.create();
    // 绑定 VAO（开始记录状态）
    Axis_VAO.bind();
    // 绑定 VBO 并上传空数据
    Axis_VBO.bind();
    Axis_VBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    Axis_VBO.allocate(nullptr, 0);

    // 绑定 EBO 并上传数据
    // EBO 必须在 VAO绑定的时候绑定
    Axis_EBO.bind();
    Axis_EBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    Axis_EBO.allocate(indices, sizeof(indices));

    // 设置顶点属性规则（记录到 VAO 中）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));
    glEnableVertexAttribArray(1);
    // 解绑 VBO（可选，但推荐）
    Axis_VBO.release();
    // 解绑 VAO（防止意外修改）
    Axis_VAO.release();

    // 初始化Data_VBO，这一步先不关联VAO
    // 先上传空数据给GPU，在读入文件数据时重新 allocate
    Data_VBO.create();
    Data_VBO.bind();
    Data_VBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);
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
    Surface_EBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);
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
    glEnable(GL_DEPTH_TEST);
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

    switch (m_shape) {
    case None:

        break;
    case Surface:

        Surface_VAO.bind();
        glDrawElements(GL_TRIANGLES, Surface_idx.size(), GL_UNSIGNED_INT, 0);
        Surface_VAO.release();

        break;
    default:

        Slice_VAO.bind();
        glDrawElements(GL_TRIANGLES, Slice_idx.size(), GL_UNSIGNED_INT, 0);
        Slice_VAO.release();

        break;
    }

    if (axisData.empty()) {
        return;
    }

    // 画系
    Axis_VAO.bind();
    timer_1.start(10);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    Axis_VAO.release();
    shaderProgram.release();

    QPainter painter(this);
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 10));
    if(is_draw == true){
        int x = 30, y = 50, w = 2000, h = 50;
        QRect rect(x, y, w, h);
        QLinearGradient gradient(x, y, x + w, y);

        // 绘制颜色渐变
        for (int i = 0; i <= 100; ++i) {
            double ratio = i / 100.0; // 0.00 ~ 1.0
            gradient.setColorAt(ratio, stColor(ratio * (dmax - dmin) + dmin, dmin, dmax));
        }

        painter.setBrush(gradient);
        painter.setPen(Qt::black);
        painter.drawRect(rect);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 6));
        for (int i = 0; i < 10; ++i) {
            double ratio = i / 10.0;
            int tick_x = x + static_cast<int>(ratio * w);
            float value = dmin + ratio * (dmax - dmin);
            QString text = QString::number(value, 'f', 1); // 保留1位小数
            int text_width = painter.fontMetrics().width(text);
            painter.drawText(tick_x - text_width / 2, y + h + 15, text);
            painter.drawLine(tick_x, y + h, tick_x, y + h + 5);
        }
    }
    //    int count = 0;
    //    for (const auto& v : getEdgeVertices(modelData,num_x,num_y,num_z)) {
    //        QVector4D worldPos(v.position, 1.0f);
    //        QVector4D clipPos = projection * view * model * worldPos;
    //        clipPos /= clipPos.w();
    //        if (clipPos.z() < -1.0f || clipPos.z() > 1.0f) continue;
    //        if (clipPos.x() < -1.0f || clipPos.x() > 1.0f ||
    //                clipPos.y() < -1.0f || clipPos.y() > 1.0f) continue;
    //        QPoint screenPos = projectToScreen(v.position, model, view, projection);
    //        if(count < 4 * num_x - 4) painter.drawText(screenPos, QString("%1").arg(outtoin(v.position.x(), 0), 0, 'f', 2));
    //        else if(count < 4*num_x + 4 * num_y - 8) painter.drawText(screenPos, QString("%1").arg(outtoin(v.position.y(), 0), 0, 'f', 2));
    //        else painter.drawText(screenPos, QString("%1").arg(outtoin(v.position.z(), 0), 0, 'f', 2));
    //        ++count;
    //    }
    //    painter.setPen(Qt::white);
    //    for (const auto& v : axisData) {
    //        QPoint screenPos = projectToScreen(v.position, model, view, projection);
    //        if (screenPos == QPoint(-1000, -1000)) continue;
    //        painter.drawText(screenPos, QString("(%1,%2,%3)")
    //                         .arg(outtoin(v.position.x(), 0), 0, 'f', 2)
    //                         .arg(outtoin(v.position.y(), 1), 0, 'f', 2)
    //                         .arg(outtoin(v.position.z(), 2), 0, 'f', 2));
    //    }


    painter.end();
}

// 调用完此函数后，再 update 中先绑定 Slice_VAO, 再 glDrawElements(GL_TRIANGLES, Slice_idx.size(), GL_UNSIGNED_INT, 0);
void znnwidget::getPlaneIndex(unsigned st_x, unsigned st_y, unsigned st_z, unsigned en_x, unsigned en_y, unsigned en_z, QVector<unsigned> &idx)
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
        qDebug() << "error plane";
        return;
    }

    //Slice_idx.clear();
    for (unsigned x = st_x; x < en_x; x++) {
        for (unsigned y = st_y; y < en_y; y++) {
            for (unsigned z = st_z; z < en_z; z++) {
                unsigned base = (num_y * num_z) * x + num_z * y + z;
                // 传入右上角和左下角两个三角形顶点在 Data_VBO 中的位置
                idx << base << base + d_sec << base + d_fst + d_sec;
                idx << base << base + d_fst << base + d_fst + d_sec;
            }
        }
    }
}

void znnwidget::getSurfaceIndex()
{
    if (not Surface_idx.empty()) {
        return;
    }

    makeCurrent();
    Surface_idx.clear();
    getPlaneIndex(0u, 0u, 0u, num_x - 1, num_y - 1, 0u, Surface_idx);
    getPlaneIndex(0u, 0u, 0u, num_x - 1, 0u, num_z - 1, Surface_idx);
    getPlaneIndex(0u, 0u, 0u, 0u, num_y - 1, num_z - 1, Surface_idx);
    getPlaneIndex(num_x - 1, 0u, 0u, num_x - 1, num_y - 1, num_z - 1, Surface_idx);
    getPlaneIndex(0u, num_y - 1, 0u, num_x - 1, num_y - 1, num_z - 1, Surface_idx);
    getPlaneIndex(0u, 0u, num_z - 1, num_x - 1, num_y - 1, num_z - 1, Surface_idx);

    Surface_VAO.bind();
    Surface_EBO.bind();
    Surface_EBO.allocate(Surface_idx.constData(), sizeof(unsigned) * (unsigned)(Surface_idx.size()));
    Surface_VAO.release();
    doneCurrent();
    update();
}

void znnwidget::getSliceIndex(int type, unsigned l)
{
    makeCurrent();

    Slice_idx.clear();

    switch (type) { // 保持跟下拉框中的序号一致
    case 2:         // 沿 X 轴方向移动的 YZ 平面
        getPlaneIndex(l, 0u, 0u, l, num_y - 1, num_z - 1, Slice_idx);
        break;
    case 3:         // 沿 Y 轴方向移动的 XZ 平面
        getPlaneIndex(0u, l, 0u, num_x - 1, l, num_z - 1, Slice_idx);
        break;
    case 4:         // 沿 Z 轴方向移动的 XY 平面
        getPlaneIndex(0u, 0u, l, num_x - 1, num_y - 1, l, Slice_idx);
        break;
    default:
        break;
    }

    Slice_VAO.bind();
    Slice_EBO.bind();
    Slice_EBO.allocate(Slice_idx.constData(), sizeof(unsigned) * (unsigned)(Slice_idx.size()));
    Slice_VAO.release();
    doneCurrent();
    update();
}

void znnwidget::gshData(std::vector<VertexData>& data)
{
    for (auto& v : data) {
        v.position[0] = intoout(v.position[0],0);
        v.position[1] = intoout(v.position[1],1);
        v.position[2] = intoout(v.position[2],2);
    }
}

void znnwidget::gshzb()
{
    float range[3];      // 保存每一对的范围
    //    float scale[3];      // 保存归一化后的比例
    // 计算每一对的差值
    for (int i = 0; i < 3; ++i) {
        range[i] = max_[i] - min_[i];
    }
    // 找出最大范围值
    float max_range = std::max({range[0], range[1], range[2]});
    // 按比例归一化，最大者为 1，其他按比例缩放
    Axis_x = range[0] / max_range;
    Axis_y = range[1] / max_range;
    Axis_z = range[2] / max_range;
}

std::vector<VertexData> znnwidget::getEdgeVertices(const std::vector<VertexData> &data, int Nx, int Ny, int Nz)
{
    std::vector<VertexData> result;

    // x方向边（4条），跳过 x = 0 和 x = Nx-1
    for (int x = 1; x < Nx - 1; ++x) {
        result.push_back(data[x * Ny * Nz + 0 * Nz + 0]);                  // (x, 0, 0)
        result.push_back(data[x * Ny * Nz + (Ny - 1) * Nz + 0]);          // (x, Ny-1, 0)
        result.push_back(data[x * Ny * Nz + 0 * Nz + (Nz - 1)]);          // (x, 0, Nz-1)
        result.push_back(data[x * Ny * Nz + (Ny - 1) * Nz + (Nz - 1)]);   // (x, Ny-1, Nz-1)
    }

    // y方向边（4条），跳过 y = 0 和 y = Ny-1
    for (int y = 1; y < Ny - 1; ++y) {
        result.push_back(data[0 * Ny * Nz + y * Nz + 0]);                 // (0, y, 0)
        result.push_back(data[(Nx - 1) * Ny * Nz + y * Nz + 0]);          // (Nx-1, y, 0)
        result.push_back(data[0 * Ny * Nz + y * Nz + (Nz - 1)]);          // (0, y, Nz-1)
        result.push_back(data[(Nx - 1) * Ny * Nz + y * Nz + (Nz - 1)]);   // (Nx-1, y, Nz-1)
    }

    // z方向边（4条），跳过 z = 0 和 z = Nz-1
    for (int z = 1; z < Nz - 1; ++z) {
        result.push_back(data[0 * Ny * Nz + 0 * Nz + z]);                 // (0, 0, z)
        result.push_back(data[0 * Ny * Nz + (Ny - 1) * Nz + z]);          // (0, Ny-1, z)
        result.push_back(data[(Nx - 1) * Ny * Nz + 0 * Nz + z]);          // (Nx-1, 0, z)
        result.push_back(data[(Nx - 1) * Ny * Nz + (Ny - 1) * Nz + z]);   // (Nx-1, Ny-1, z)
    }

    return result;
}

void znnwidget::drawColor(float dmi, float dma,bool f)
{
    is_draw = f;
    dmin = dmi;
    dmax = dma;
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

void znnwidget::resetData()
{
    axisData.clear();
    modelData.clear();
}

void znnwidget::processData()
{
    // 处理坐标轴
    gshzb();
    // 前面
    axisData.push_back(VertexData({ -Axis_x, -Axis_y,  Axis_z })); // 0
    axisData.push_back(VertexData({  Axis_x, -Axis_y,  Axis_z })); // 1
    axisData.push_back(VertexData({  Axis_x,  Axis_y,  Axis_z })); // 2
    axisData.push_back(VertexData({ -Axis_x,  Axis_y,  Axis_z })); // 3
    // 后面
    axisData.push_back(VertexData({ -Axis_x, -Axis_y, -Axis_z })); // 4
    axisData.push_back(VertexData({  Axis_x, -Axis_y, -Axis_z })); // 5
    axisData.push_back(VertexData({  Axis_x,  Axis_y, -Axis_z })); // 6
    axisData.push_back(VertexData({ -Axis_x,  Axis_y, -Axis_z })); // 7
    Axis_VBO.bind();
    Axis_VBO.allocate(axisData.data(), sizeof(VertexData) * (unsigned)(axisData.size()));
    Axis_VBO.release();

    gshData(modelData);

    qDebug() << modelData.size() << '\n';

    Data_VBO.bind();
    Data_VBO.allocate(modelData.data(), sizeof(VertexData) * (unsigned)(modelData.size()));
    Data_VBO.release();
}
QColor znnwidget::stColor(float a, float mn, float mx) {
    // 根据速度的最大值和最小值将该节点的速度值归一化到[0.0, 1.0]
    double t = (a - mn) / (mx - mn);
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    // 使用 HSL 颜色进行过度
    // 计算色相(240.0为蓝 360.0为红)
    double hue = 240.0 + 120.0 * t;

    QColor tmp = QColor::fromHslF(hue / 360.0, 1.0, 0.5);

    return tmp;
}
znnwidget::~znnwidget()
{
    if(!isValid()) return;
    makeCurrent();
    Axis_VAO.destroy();
    Axis_VBO.destroy();
    Axis_EBO.destroy();

    Data_VBO.destroy();
    Surface_VAO.destroy();
    Surface_EBO.destroy();
    Slice_VAO.destroy();
    Slice_EBO.destroy();
    doneCurrent();
}
