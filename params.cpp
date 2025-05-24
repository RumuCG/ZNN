#include "params.h"
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
Params::Params(QObject *parent) :
    QObject(parent)
{
    outputFileName = "velocity_model.txt";
    inputFileName = "";
    min_v = 0.0f;
    max_v = 0.0f;

    for (int i = 0; i < 3; i++) {
        axisMin[i] = 0.0f;
        axisStep[i] = 0.0f;
        axisCount[i] = 0;
    }

    interpPower = 0.0f;
    radius = 0.0f;
    minPoint = 0.0f;
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

bool Params::chkData()
{
    for (int i = 0; i < 3; i++) {
        qDebug() << axisMin[i] << ' ' << axisStep[i] << ' ' << axisCount[i];
    }
    long long scale = 1ll * axisCount[0] * axisCount[1] * axisCount[2];
    if (scale == 0 || scale > LimitScale) { // 数据规模问题
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("参数警告");
        msgBox.setText(QString("数据规模 %1 不在有效范围 [1 - %2] 之间！")
                      .arg(scale).arg(LimitScale));
        msgBox.setInformativeText("请调整输入参数");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }

    if (axisStep[0] * axisStep[0] * axisStep[0] == 0.0f) { // 数据步长问题
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("参数警告");
        msgBox.setText(QString("数据的步长不能为 0"));
        msgBox.setInformativeText("请调整输入参数");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }
//    return false;
    return true;
}

bool Params::writeConfig()
{
    // 检查数据合法性
    if (!chkData()) {
        return false;
    }

    // 根据参数创建config.ini
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

