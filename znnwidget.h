#ifndef ZNNWIDGET_H
#define ZNNWIDGET_H

#include <QWidget>
#include<QOpenGLWidget>
#include<QOpenGLFunctions_3_3_Core>

class znnwidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    enum Shape{None,Rect,Circle,Triangle};
    explicit znnwidget(QWidget *parent = nullptr);
    void drawshape(Shape shape);// 对外接口
    void frameline(bool is_frame);
    ~znnwidget();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
signals:
private:
    Shape m_shape;//保存需要绘制的图像
public slots:
};

#endif // ZNNWIDGET_H
