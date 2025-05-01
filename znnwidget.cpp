#include "znnwidget.h"
unsigned int VBO,VAO,EBO;
//VBO：存数据（What）
//VAO：定义规则（How）
//EBO: 重复顶点资源节省
//绑定 VAO
//绑定 VBO
//上传数据到 VBO
//设置顶点属性规则
//解绑 VBO 和 VAO
float vertices[] = {
    // 位置               // 纹理坐标
    // 前面
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,

    // 后面
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,

    // 左面
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,

    // 右面
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 1.0f,

    // 上面
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,

    // 下面
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,
};


unsigned int indices[] = {
    0, 1, 2,   0, 2, 3,       // 前面
    4, 5, 6,   4, 6, 7,       // 后面
    8, 9,10,   8,10,11,       // 左面
   12,13,14,  12,14,15,       // 右面
   16,17,18,  16,18,19,       // 上面
   20,21,22,  20,22,23        // 下面
};

znnwidget::znnwidget(QWidget *parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(&timer_1,SIGNAL(timeout()),this,SLOT(on_timeout()));
}

void znnwidget::drawshape(znnwidget::Shape shape)
{
    m_shape = shape;
    update();
}

void znnwidget::frameline(bool is_frame)
{
    makeCurrent();
    if(is_frame)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    update();
    doneCurrent();
}

void znnwidget::dy(bool is_dy)
{
    //makeCurrent();
    if(is_dy)
        timer_1.start(100);
    else
        timer_1.stop();
    //doneCurrent();
    update();
}


void znnwidget::initializeGL()
{
    // 创建 VAO 和 VBO 和 EBO
    initializeOpenGLFunctions();

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
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 绑定 EBO 并上传数据
    // EBO 必须在 VAO绑定的时候 绑定
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    // 设置顶点属性规则（记录到 VAO 中）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    QImage img(":/images/wall.png");
    //img = img.convertToFormat(QImage::Format_RGBA8888);
    // 注意：Qt5.9的QImage默认是**左上角原点**，OpenGL是**左下角原点**
    // 需要 flip 垂直翻转一下：
    img = img.mirrored();
    textureWall = new QOpenGLTexture(img);

    // 解绑 VBO（可选，但推荐）
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 解绑 VAO（防止意外修改）
    glBindVertexArray(0);
}

void znnwidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h)
}

/*void znnwidget::paintGL()
{
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    switch (m_shape) {
    case Rect:
        textureWall -> bind();
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

        break;
    default:
        break;
    }
    //glDrawArrays(GL_TRIANGLES,0,3);
    //glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

}*/
void znnwidget::paintGL()
{
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram.bind();
    QMatrix4x4 model;
    model.translate(offsetX, offsetY, 0.0f);
    model.rotate(angle, 1.0f, 1.0f, 0.0f); // 多轴旋转看起来更立体
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
        textureWall->bind();
        timer_1.start(10);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        break;
    default:
        break;
    }
}

void znnwidget::on_timeout()
{
    makeCurrent();
    if(is_xc) angle += 2.0f;
    if (angle >= 360.0f) angle -= 360.0f;
    //offsetX += 0.02f; // 每帧右移一点
    //if (offsetX > 1.0f) offsetX = -1.0f; // 循环移动
    shaderProgram.bind();

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
