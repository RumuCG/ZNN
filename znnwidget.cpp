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
    // 4 个顶点坐标
    -0.5f,  0.5f, 0.0f, // 0: A
    -0.5f, -0.5f, 0.0f, // 1: B
    0.5f, -0.5f, 0.0f, // 2: C
    0.5f,  0.5f, 0.0f  // 3: D
};
unsigned int indices[] = {
    0, 1, 2,  // 第一个三角形
    0, 2, 3   // 第二个三角形
};
znnwidget::znnwidget(QWidget *parent) : QOpenGLWidget(parent)
{
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

void znnwidget::paintGL()
{
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    switch (m_shape) {
    case Rect:
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        break;
    default:
        break;
    }
    //glDrawArrays(GL_TRIANGLES,0,3);
    //glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
}

void znnwidget::on_timeout()
{
    makeCurrent();
    GLfloat value = QTime::currentTime().second();
    // qDebug() << value;
    value = sin(value)/2.0f + 0.5f;
    shaderProgram.setUniformValue("utime",value);
    doneCurrent();
    update();
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
