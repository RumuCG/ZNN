#include "params.h"
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
Params::Params(QObject *parent) :
    QObject(parent)
{
    outputFileName = "velocity_model.txt";
    inputFileName = "1-7well.txt";
    min_v = 5200.0f;
    max_v = 3700.0f;

    // test start

    axisMin[0] = 0.0f;
    axisMin[1] = -500.0f;
    axisMin[2] = 2640.0f;

    axisStep[0] = axisStep[1] = axisStep[2] = 40.0f;

    axisCount[0] = axisCount[1] = 25;
    axisCount[2] = 24;

    interpPower = 2.0f;
    radius = 50.0f;
    minPoint = 5.0f;

    // test end
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

int Params::chkData()
{
    return 0;
}

bool Params::writeConfig()
{
    // 检查数据合法性
    switch (chkData()) {
    case 0:
        break;
    default:
        return false;
    }

    QFile file(QCoreApplication::applicationDirPath() + "/config.ini");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "fail to open " << (QCoreApplication::applicationDirPath() + "/config.ini");
        return false;
    }

    QTextStream out(&file);

    out << "# config\n";
    out << "# IO\n";
    out << "input_file " << inputFileName << '\n';
    out << "output_file " << outputFileName << '\n';
    out << '\n';

    out << "# X grid\n";
    out << "x_min " << axisMin[0] << '\n';
    out << "x_step " << axisStep[0] << '\n';
    out << "x_count " << axisCount[0] << '\n';
    out << '\n';

    out << "# Y grid\n";
    out << "y_min " << axisMin[1] << '\n';
    out << "y_step " << axisStep[1] << '\n';
    out << "y_count " << axisCount[1] << '\n';
    out << '\n';

    out << "# Z grid\n";
    out << "z_min " << axisMin[2] << '\n';
    out << "z_step " << axisStep[2] << '\n';
    out << "z_count " << axisCount[2] << '\n';
    out << '\n';

    out << "# IPL control\n";
    out << "interp_power " << interpPower << '\n';
    out << "radius " << radius << '\n';
    out << "min_points " << minPoint;

    file.close();
    return true;
}

