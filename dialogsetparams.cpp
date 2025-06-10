#include "dialogsetparams.h"
#include "ui_dialogsetparams.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

DialogSetParams::DialogSetParams(QWidget *parent) :
    QDialog(parent),
    BaseParams (true),      // 启用参数初始化
    ui(new Ui::DialogSetParams)
{
    ui->setupUi(this);
    interpPower = 2.0;
    radius = 50.0;
    minPoints = 5;

    ui->doubleSpinBox_X_Min->setValue(static_cast<double>(axisMin[0]));
    ui->doubleSpinBox_X_Step->setValue(static_cast<double>(axisStep[0]));
    ui->spinBox_X_Count->setValue(axisCount[0]);

    ui->doubleSpinBox_Y_Min->setValue(static_cast<double>(axisMin[1]));
    ui->doubleSpinBox_Y_Step->setValue(static_cast<double>(axisStep[1]));
    ui->spinBox_Y_Count->setValue(axisCount[1]);

    ui->doubleSpinBox_Z_Min->setValue(static_cast<double>(axisMin[2]));
    ui->doubleSpinBox_Z_Step->setValue(static_cast<double>(axisStep[2]));
    ui->spinBox_Z_Count->setValue(axisCount[2]);

    ui->doubleSpinBox_interp_power->setValue(static_cast<double>(interpPower));
    ui->doubleSpinBox_radius->setValue(static_cast<double>(radius));
    ui->spinBox_min_points->setValue(minPoints);

    ui->label_outputFile->setText(DefaultOutFile);
}

DialogSetParams::~DialogSetParams()
{
    delete ui;
}

void DialogSetParams::getParams()
{
    inputFile = ui->label_inputFile->text();
    outputFile = ui->label_outputFile->text();

    axisMin[0] = static_cast<float>(ui->doubleSpinBox_X_Min->value());
    axisStep[0] = static_cast<float>(ui->doubleSpinBox_X_Step->value());
    axisCount[0] = ui->spinBox_X_Count->value();

    axisMin[0] = static_cast<float>(ui->doubleSpinBox_X_Min->value());
    axisStep[0] = static_cast<float>(ui->doubleSpinBox_X_Step->value());
    axisCount[0] = ui->spinBox_X_Count->value();

    axisMin[0] = static_cast<float>(ui->doubleSpinBox_X_Min->value());
    axisStep[0] = static_cast<float>(ui->doubleSpinBox_X_Step->value());
    axisCount[0] = ui->spinBox_X_Count->value();

    interpPower = static_cast<float>(ui->doubleSpinBox_interp_power->value());
    radius = static_cast<float>(ui->doubleSpinBox_radius->value());
    minPoints = ui->spinBox_min_points->value();;
}

QString DialogSetParams::getExe()
{
    return ui->exeBox->currentText();
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
        QMessageBox::information(this, "提示", "只有当拓展名为 .vpr时才能被正确识别为模型文件");
    }
}


bool DialogSetParams::chkData()
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

    if (inputFile.size() == 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("参数警告");
        msgBox.setText(QString("还未选择源文件！"));
        msgBox.setInformativeText("请先选择源文件");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }
    if (!inputFile.endsWith(".txt", Qt::CaseInsensitive)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("参数警告");
        msgBox.setText(QString("源文件错误"));
        msgBox.setInformativeText("源文件必须是.txt结尾的文本文件，请重新选择");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }

    if (outputFile == DefaultOutFile) {
        // 保存在默认路径
        // 即同 inputFile 相同的路径
        QFileInfo fileInfo(inputFile);

        outputFile = fileInfo.dir().filePath(fileInfo.completeBaseName() + ".vpr");
    }

    return true;
}

bool DialogSetParams::writeConfig()
{
    // 检查数据合法性
    if (!chkData()) {
        return false;
    }

    // 根据参数创建config.ini
    QFile file(QCoreApplication::applicationDirPath() + "/Interpolation/config.ini");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "fail to open " << (QCoreApplication::applicationDirPath() + "/Interpolation/config.ini");
        return false;
    }

    QTextStream out(&file);

    out << "# config\n";
    out << "# IO\n";
    out << "input_file " << inputFile << '\n';
    out << "output_file " << outputFile << '\n';
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
    out << "min_points " << minPoints;

    file.close();
    return true;
}
