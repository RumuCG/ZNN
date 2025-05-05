#ifndef ZNNWIDGET_H
#define ZNNWIDGET_H

#include <QWidget>
#include<QOpenGLWidget>
#include<QOpenGLFunctions_3_3_Core>
#include<QOpenGLShaderProgram>
#include<QTimer>
#include<QTime>
#include<QOpenGLTexture>
#include <QKeyEvent>
#include <bits/stdc++.h>
#include <QPainter>
struct VertexData {
    QVector3D position;
    QVector3D color;
};
class znnwidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    enum Shape{None,Rect,Circle,Triangle};
    explicit znnwidget(QWidget *parent = nullptr);
    void drawshape(Shape shape);// 对外接口
    void frameline(bool is_frame);
    void dy(bool is_dy);
    void keyPressEvent(QKeyEvent *event);
    bool is_xc = false;
    std::vector<VertexData> vertices;
    QPoint projectToScreen(const QVector3D &worldPos, const QMatrix4x4 &model, const QMatrix4x4 &view, const QMatrix4x4 &proj);
    ~znnwidget();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
signals:
private:
    Shape m_shape;//保存需要绘制的图像
    QOpenGLShaderProgram shaderProgram;
    QTimer timer_1;
    float angle = 0.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

public slots:
    void on_timeout();
};

#endif // ZNNWIDGET_H
