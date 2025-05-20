#ifndef ZNNWIDGET_H
#define ZNNWIDGET_H
#include <QWidget>
#include<QOpenGLWidget>
#include<QOpenGLFunctions_3_3_Core>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>
#include<QTimer>
#include<QTime>
#include <QMatrix4x4>
#include<QOpenGLTexture>
#include <QKeyEvent>
#include <bits/stdc++.h>
#include <QPainter>
#include <QVector>
struct VertexData {
    QVector3D position;
    QVector3D color;
};
class znnwidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    float angleX = 0.0f; // 旋转角度 各轴
    float angleY = 0.0f;
    float angleZ = 0.0f;
    float offsetX = 0.0f;// 上移量
    float offsetY = 0.0f; // 下移量
    float scalen = 1.0f; // 缩放比例
    enum Shape{None,Rect,Circle,Triangle};
    explicit znnwidget(QWidget *parent = nullptr);
    void drawshape(Shape shape);// 对外接口
    void frameline(bool is_frame);
    void dy(bool is_dy);
    void recover(); // 复原坐标系
    void keyPressEvent(QKeyEvent *event);
    bool is_xc = false;
    std::vector<VertexData> vertices;
    QPoint projectToScreen(const QVector3D &worldPos, const QMatrix4x4 &model, const QMatrix4x4 &view, const QMatrix4x4 &proj);

    unsigned num_x, num_y, num_z; // 离散化后是一个 num_x * num_y * num_z 的三维数组，这三个值需要读入数据后算出
    // 传入的 st 和 en 都是三维数组中的点，即在[ (0, 0, 0), (num_x, num_y, num_z) )里的整数坐标
    // 全闭 [st, en]
    void DrawPlane(unsigned st_x, unsigned st_y, unsigned st_z, unsigned en_x, unsigned en_y, unsigned en_z);
    void test();
    void release();
    ~znnwidget();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
signals:
private:
    static constexpr float Axis_x = 0.75f, Axis_y = 0.5f, Axis_z = 0.5f; // 轴的范围
    Shape m_shape;//保存需要绘制的图像
    QOpenGLShaderProgram shaderProgram;
    QTimer timer_1;

    QOpenGLVertexArrayObject Surface_VAO, Slice_VAO;
    QOpenGLBuffer Data_VBO;     // 静态数据，有序存储所有点的坐标和颜色
    // 以下两个EBO共用上面一个VBO
    QOpenGLBuffer Surface_EBO;  // 静态数据，用于画整个立方体（因为不涉及到裁剪之类的，所以当需要画立方体时，直接用这个不用重新找点）
    QVector<unsigned> Surface_idx;
    QOpenGLBuffer Slice_EBO;    // 动态数据，用于画指定平面（因为平面是变化的，所以每次画平面都需要重新找点更新这个）
    QVector<unsigned> Slice_idx;
public slots:
    void on_timeout();
};

#endif // ZNNWIDGET_H
