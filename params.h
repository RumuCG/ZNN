#ifndef PARAMS_H
#define PARAMS_H
#include <QObject>
#include <QVector3D>
/*
存放与差值程序交互的数据信息
*/
class Params : public QObject
{
    Q_OBJECT
public:
    Params(QObject *parent = nullptr);

    QString inputFileName;
    QString outputFileName;

    // 数据规模信息
    // 0 x轴
    // 1 y轴
    // 2 z轴
    float axisMin[3];
    float axisStep[3];
    int axisCount[3];

    bool inRange(int i, float val);   // 判断 val 是否再第 i 根轴上
    float getDiff(int i);           // 返回第 i 根轴上最值之差
    int getScale();                 // 返回数据规模 x * y * z
    QVector3D getRealPos(int pos);  // 根据pos的值计算在网格中的坐标，根据在网格中的坐标计算实际坐标

    // 数据的最值
    float min_v, max_v;
    void setValueRange(float min, float max);      // 设置以上两个最值

    // IPL信息 // 不懂
    float interpPower;
    float radius;
    int minPoint;

    bool writeConfig();              // 写数据到 config.ini

private:
    static constexpr long long LimitScale = 1e10; // 1e7
    bool chkData();                   // 检查参数的合法性
};

#endif // PARAMS_H
