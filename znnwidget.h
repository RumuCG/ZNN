
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
#include "params.h"

struct VertexData {
    QVector3D position;
    QVector3D color;

    // 默认颜色是白色
    VertexData(QVector3D pos = { 0.0f, 0.0f, 0.0f }, QColor c = QColor(255, 255, 255)) {
        position = pos;
        color = { static_cast<float>(c.redF()), static_cast<float>(c.greenF()), static_cast<float>(c.blueF()) };
    }
};

class znnwidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit znnwidget(QWidget *parent = nullptr);
    // 初始化参数类（存放与差值程序交互的数据信息），时znnwidget中的参数类指针和mainwindow中的参数类指针指向同一个地方
    // 以同步数据
    void initParams(Params *p = nullptr);

    float angleX = 90.0f; // 旋转角度 各轴
    float angleY = 0.0f;
    float angleZ = 300.0f;
    float offsetX = 0.0f;// 上移量
    float offsetY = 0.0f; // 下移量
    float scalen = 0.5f; // 缩放比例
    enum Shape{ None, Surface, SliceXY, SliceXZ, SliceYZ, CntShape };
    void setShape(Shape shape);// 对外接口

    void recover(); // 复原坐标系
    void keyPressEvent(QKeyEvent *event);
    bool is_xc = false;
    bool is_draw = false;
    std::vector<VertexData> axisData;  // 坐标系再屏幕上的位置
    std::vector<VertexData> modelData; // 每个点的数据
    void resetData();                  // 重置所有数据，并预存空间
    void getData(int pos, float v);
    void processData();                // 处理读入的数据

    QPoint projectToScreen(const QVector3D &worldPos, const QMatrix4x4 &model, const QMatrix4x4 &view, const QMatrix4x4 &proj);
    float intoout(float x,int p);
    float outtoin(float x,int p);

    // 传入的 st 和 en 都是三维数组中的点，即在[ (0, 0, 0), (num_x, num_y, num_z) )里的整数坐标
    // 全闭 [st, en]
    void getPlaneIndex(unsigned st_x, unsigned st_y, unsigned st_z, unsigned en_x, unsigned en_y, unsigned en_z, QVector<unsigned> &idx);
    void getSurfaceIndex();
    void getSliceIndex(int type, unsigned l);
    void gshData(std::vector<VertexData>&);
    void gshzb();
    std::vector<unsigned> slice_situation;
    std::vector<VertexData> getEdgeVertices(const std::vector<VertexData>& data, int Nx, int Ny, int Nz);
    QColor stColor(float a, float mn, float mx);
    std::vector<unsigned> corners;
    ~znnwidget();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
signals:

private:
    Params *params;

    float Axis_[3]; // 轴的范围

    Shape m_shape = None;  //保存需要绘制的图像

    QTimer timer_1;

    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject Axis_VAO, Surface_VAO, Slice_VAO;
    QOpenGLBuffer Axis_VBO, Axis_EBO;
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
