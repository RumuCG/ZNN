#include "params.h"
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
Params::Params(QObject *parent) :
    QObject(parent),
    BaseParams (false)      // 不启用参数初始化
{
}

bool Params::inRange(int i, float val)
{
    if (i < 0 || 3 <= i) {
        return false;
    }
    return axisMin[i] <= val && val <= axisMin[i] + (axisCount[i] - 1) * axisStep[i];
}

float Params::getDiff(int i)
{
    return (axisCount[i] - 1) * axisStep[i];
}

int Params::getScale()
{
    return axisCount[0] * axisCount[1] * axisCount[2];
}

QVector3D Params::getRealPos(int pos)
{
    int z = pos % axisCount[2];
    pos /= axisCount[2];
    int y = pos % axisCount[1];
    pos /= axisCount[1];
    int x = pos;
    return { axisMin[0] + 1.0f * x * axisStep[0], axisMin[1] + 1.0f * y * axisStep[1], axisMin[2] + 1.0f * z *axisStep[2] };
}

void Params::setValueRange(float min, float max)
{
    min_v = min;
    max_v = max;
}

