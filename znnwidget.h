
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
#include "vertexdata.h"

class znnwidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit znnwidget(QWidget *parent = nullptr);

    float angleX = 0.0f; // 旋转角度 各轴
    float angleY = 0.0f;
    float angleZ = 0.0f;
    float offsetX = 0.0f;// 上移量
    float offsetY = 0.0f; // 下移量
    float scalen = 1.0f; // 缩放比例
    enum Shape{ None, Surface, SliceXY, SliceXZ, SliceYZ, CntShape };
    void setShape(Shape shape);// 对外接口

    void frameline(bool is_frame);
    void dy(bool is_dy);

    void recover(); // 复原坐标系
    void keyPressEvent(QKeyEvent *event);
    bool is_xc = false;

    std::vector<VertexData> axisData;  // 坐标系再屏幕上的位置
    std::vector<VertexData> modelData; // 每个点的数据
//    unsigned num_x, num_y, num_z;
    unsigned num_x = 25, num_y = 25, num_z = 24; // 测试数据的规模
    // 离散化后是一个 num_x * num_y * num_z 的三维数组，这三个值需要读入数据后算出
    // 0，1，2分别是x，y，z三个方向的最大最小值
//    float min_[3], max_[3];
    float min_[3] = { 0.0f, -500.0f, 2640.0f }, max_[3] = { min_[0] + 40.0f * (num_x - 1), min_[1] + 40.0f * (num_y - 1), min_[2] + 40.0f * (num_z - 1) }; // 测试数据的步长为 40
    void resetData();
    void processData();

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
    std::vector<VertexData> getEdgeVertices(const std::vector<VertexData>& data, int Nx, int Ny, int Nz);
    void drawColor(float dmi,float dma,bool f);
    bool is_draw = false;
    QColor stColor(float a, float mn, float mx);
    float dmin = 0, dmax = 0;
    ~znnwidget();
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
signals:
private:
    float Axis_x = 0.75f, Axis_y = 0.5f, Axis_z = 0.5f; // 轴的范围

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
