#include "dialogsetparams.h"
#include "ui_dialogsetparams.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

DialogSetParams::DialogSetParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetParams)
{
    ui->setupUi(this);
}

DialogSetParams::~DialogSetParams()
{
    delete ui;
}

QString DialogSetParams::inputFile()
{
    return ui->label_inputFile->text();
}

QString DialogSetParams::outputFile()
{
    return ui->label_outputFile->text();
}

float DialogSetParams::axisMin(int id)
{
    double res = 0.0;
    switch (id) {
    case 0:
        res = ui->doubleSpinBox_X_Min->value();
        break;
    case 1:
        res = ui->doubleSpinBox_Y_Min->value();
        break;
    case 2:
        res = ui->doubleSpinBox_Z_Min->value();
        break;
    default:
        qDebug() << "传入了错误的参数给函数 DialogSetParams::axisMin(int)";
        exit(1);
    }
    return static_cast<float>(res);
}

float DialogSetParams::axisStep(int id)
{
    double res = 0.0;
    switch (id) {
    case 0:
        res = ui->doubleSpinBox_X_Step->value();
        break;
    case 1:
        res = ui->doubleSpinBox_Y_Step->value();
        break;
    case 2:
        res = ui->doubleSpinBox_Z_Step->value();
        break;
    default:
        qDebug() << "传入了错误的参数给函数 DialogSetParams::axisStep(int)";
        exit(1);
    }
    return static_cast<float>(res);
}

int DialogSetParams::axisCount(int id)
{
    int res = 0;
    switch (id) {
    case 0:
        res = ui->spinBox_X_Count->value();
        break;
    case 1:
        res = ui->spinBox_Y_Count->value();
        break;
    case 2:
        res = ui->spinBox_Z_Count->value();
        break;
    default:
        qDebug() << "传入了错误的参数给函数 DialogSetParams::axisCount(int)";
        exit(1);
    }
    return res;
}

float DialogSetParams::ipow()
{
    return static_cast<float>(ui->doubleSpinBox_interp_power->value());
}

float DialogSetParams::radius()
{
    return static_cast<float>(ui->doubleSpinBox_radius->value());
}

int DialogSetParams::minPoints()
{
    return ui->spinBox_min_points->value();
}

// 选择模型文件的源文件(井文件)
void DialogSetParams::on_BtnChooseInput_clicked()
{
    // 打开文件对话框
    QString filePath = QFileDialog::getOpenFileName(
                this,
                "选择源文件",
                QDir::homePath(), // 默认目录
                "文本文件 (*.txt)" // 过滤器
                );

    if (!filePath.isEmpty()) {
        ui->label_inputFile->setText(filePath);
    }
}

// 选择模型文件的保存路径
void DialogSetParams::on_BtnSaveAs_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(
                this,
                "将模型文件保存为",
                QDir::homePath() + "/untitiled.vpr",
                tr("模型文件(*.vpr)")
                );

    if (filePath.isEmpty()) {
        return;
    }

    // 若检测到用户要保存为已存在的文件，则询问是否覆盖
    QFile file(filePath);
    if (file.exists() && QMessageBox::question(this, "文件已存在", "是否覆盖已存在的文件？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    ui->label_outputFile->setText(filePath);

    if (!filePath.endsWith(".vpr", Qt::CaseInsensitive)) {
        QMessageBox::information(this, "提示", "只有当拓展名为 .vp r时才能被正确识别为模型文件");
    }
}
