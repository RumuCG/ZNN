#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include "params.h"
#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    params(new Params(this))
{
    qDebug() << "init params->max_v = " << params->max_v << '\n';
    this->setStyleSheet("background-color: rgb(21, 160, 221);");  // 浅紫色（plum）
    qDebug() << "in setupui";
    ui->setupUi(this);
    qDebug() << "out setupui";
    ui->openGLWidget->initParams(params);
    ui->statusBar->showMessage("当前未读取文件");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MainWindow::on_drawModel_triggered()
{
    if (not params->writeConfig()) {
        qDebug() << "fail to write config.ini";
    }
    else {
        qDebug() << "write config.ini successfully";

        // 尝试启动差值程序
        QString programPath = QCoreApplication::applicationDirPath() + "/1DIPL.exe";
        if (!QFileInfo::exists(programPath)) {
            qWarning() << "1DIPL.exe 不存在：" << programPath;
            return;
        }

        QProcess process;
        process.setWorkingDirectory(QCoreApplication::applicationDirPath());
        process.execute(programPath, QStringList()); // 传递空参数给 1DIPL.exe

        if (!process.waitForStarted(3000)) {
            qWarning() << "程序启动失败：" << process.errorString();
            return;
        }
        if (!process.waitForFinished(-1)) {
            qWarning() << "程序执行异常：" << process.errorString();
            return;
        }

        int exitCode = process.exitCode();
        qDebug() << "执行完成，退出码：" << exitCode;

        if (exitCode == 0) {    // 正常退出才开始读取文件
            fileRead = readFile(QCoreApplication::applicationDirPath() + "/" +params->outputFileName);
        }
    }
}

void MainWindow::on_actxc_triggered()
{
    ui->openGLWidget->is_xc = ui->actxc->isChecked();
    update();
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->openGLWidget->scalen = float(value)*0.02f;
    update();
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    ui->openGLWidget->angleX = value;
    update();
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    ui->openGLWidget->angleY = value;
    update();
}

void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    ui->openGLWidget->angleZ = value;
    update();
}

void MainWindow::on_recover_button_clicked()
{
    ui->openGLWidget->recover();
    ui->horizontalSlider->setValue(50);
    ui->horizontalSlider_2->setValue(0);
    ui->horizontalSlider_3->setValue(0);
    ui->horizontalSlider_4->setValue(0);
    update();
}

void MainWindow::on_DisplayMode_currentIndexChanged(int index)
{
    bool enable = (index > 1);
    ui->PostionSlider->setEnabled(enable);
//    ui->PositionBox->setEnabled(enable);

    if (index == 0) {   // NULL 啥都不干
        ui->PostionSlider->setValue(0);
        ui->openGLWidget->setShape(ui->openGLWidget->None);
        ui->openGLWidget->update();
    }
    else if (not fileRead) { // 未读入文件时，禁止选择其他选项
        ui->DisplayMode->setCurrentIndex(0);
    }
    switch (index) {
    case 1:             // A 显示全部 6 个面
        ui->PostionSlider->setValue(0);
        ui->openGLWidget->setShape(ui->openGLWidget->Surface);
        ui->openGLWidget->getSurfaceIndex();

        break;
    case 2:             // X 沿X轴显示切片
        ui->PostionSlider->setValue(0);
        ui->PostionSlider->setMinimum(0);
        ui->PostionSlider->setMaximum(params->axisCount[0] - 1);
        // qDebug() << ui->openGLWidget->num_x;
        ui->openGLWidget->setShape(ui->openGLWidget->SliceYZ);
        ui->openGLWidget->getSliceIndex(2, 0u);

        break;
    case 3:             // Y 沿Y轴显示切片
        ui->PostionSlider->setValue(0);
        ui->PostionSlider->setMinimum(0);
        ui->PostionSlider->setMaximum(params->axisCount[1] - 1);
        ui->openGLWidget->setShape(ui->openGLWidget->SliceXZ);
        ui->openGLWidget->getSliceIndex(3, 0u);

        break;
    case 4:             // Z 沿Z轴显示切片
        ui->PostionSlider->setValue(0);
        ui->PostionSlider->setMinimum(0);
        ui->PostionSlider->setMaximum(params->axisCount[2] - 1);
        ui->openGLWidget->setShape(ui->openGLWidget->SliceXY);
        ui->openGLWidget->getSliceIndex(4, 0u);

        break;
    default:
        break;
    }
}

void MainWindow::on_PostionSlider_valueChanged(int p)
{
    int index = ui->DisplayMode->currentIndex();
    switch (index) {
    case 2:             // X 沿X轴显示切片
        //ui->PostionSlider->setValue(0);
        ui->openGLWidget->setShape(ui->openGLWidget->SliceYZ);
        ui->openGLWidget->getSliceIndex(2, p);

        break;
    case 3:             // Y 沿Y轴显示切片
        //ui->PostionSlider->setValue(0);
        ui->openGLWidget->setShape(ui->openGLWidget->SliceXZ);
        ui->openGLWidget->getSliceIndex(3, p);

        break;
    case 4:             // Z 沿Z轴显示切片
        //ui->PostionSlider->setValue(0);
        ui->openGLWidget->setShape(ui->openGLWidget->SliceXY);
        ui->openGLWidget->getSliceIndex(4, p);

        break;
    default:
        break;
    }
}

bool MainWindow::readFile(const QString &FileName)
{
    qDebug() << "reading File\n";
    // 以只读方式打开数据文件
    QFile dataFile(FileName);
    qDebug() << "ready to read " << FileName << '\n';
    if (not dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) { // QIODevice::Text 处理换行符号
        qDebug() << "Fail to read File " << FileName << '\n';
        return false;
    }
    QTextStream in(&dataFile);
    QStringList lines = in.readAll()                                // 读取所有行
                          .split('\n', QString::SkipEmptyParts);    // 只将非空行存到lines里面


    ui->openGLWidget->resetData();

    unsigned pos = 0; // 记录坐标
    bool headLine = true; // 特殊处理第一行
    for (const QString &line : lines) {
        QStringList valueList = line.split(' ', QString::SkipEmptyParts);
        if (valueList.empty()) {
            continue;
        }

//        if (headLine) {                         // 第一行读入最值
//            params->setValueRange(valueList[0].toFloat(), valueList[1].toFloat());
//            headLine = false;
//        }
//        else {
//            for (const QString &v : valueList) {
//                ui->openGLWidget->getData(pos++, v.toFloat());
//            }
//        }
        for (const QString &v : valueList) {
            ui->openGLWidget->getData(pos++, v.toFloat());
        }
    }

    qDebug() << "File read ok !\n" << ui->openGLWidget->modelData.size() << " datas Read\n";

    ui->openGLWidget->processData();

    dataFile.close();
    return true;
}

void MainWindow::on_colorshow_triggered()
{
//    ui->openGLWidget->is_draw = ui-> colorshow ->isChecked();
//    update();
}

// <<---                根据界面输入值设定对应的参数              --->>

void MainWindow::on_doubleSpinBox_X_Min_editingFinished()
{
    params->axisMin[0] = static_cast<float>(ui->doubleSpinBox_X_Min->value());
}
void MainWindow::on_doubleSpinBox_X_Step_editingFinished()
{
    params->axisStep[0] = static_cast<float>(ui->doubleSpinBox_X_Step->value());
}
void MainWindow::on_spinBox_X_Count_editingFinished()
{
    params->axisCount[0] = ui->spinBox_X_Count->value();
}

void MainWindow::on_doubleSpinBox_Y_Min_editingFinished()
{
    params->axisMin[1] = static_cast<float>(ui->doubleSpinBox_Y_Min->value());
}
void MainWindow::on_doubleSpinBox_Y_Step_editingFinished()
{
    params->axisStep[1] = static_cast<float>(ui->doubleSpinBox_Y_Step->value());
}
void MainWindow::on_spinBox_Y_Count_editingFinished()
{
    params->axisCount[1] = ui->spinBox_Y_Count->value();
}

void MainWindow::on_doubleSpinBox_Z_Min_editingFinished()
{
    params->axisMin[2] = static_cast<float>(ui->doubleSpinBox_Z_Min->value());
}
void MainWindow::on_doubleSpinBox_Z_Step_editingFinished()
{
    params->axisStep[2] = static_cast<float>(ui->doubleSpinBox_Z_Step->value());
}
void MainWindow::on_spinBox_Z_Count_editingFinished()
{
    params->axisCount[2] = ui->spinBox_Z_Count->value();
}

// <<---                根据界面输入值设定对应的参数              --->>

void MainWindow::on_openFile_triggered()
{
    // 获取应用目录下的1DIPL文件夹路径
    QString dataDir = QDir(QCoreApplication::applicationDirPath())
                     .filePath("1DIPL");

    // 如果不存在就创建
    QDir().mkpath(dataDir);

    // 打开文件对话框
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择源文件",
        dataDir,  // 初始目录
        "文本文件 (*.txt)" // 过滤器
    );

    if (!fileName.isEmpty()) {
        params->inputFileName = fileName;
        ui->statusBar->showMessage("当前文件：" + fileName);
    }
}
