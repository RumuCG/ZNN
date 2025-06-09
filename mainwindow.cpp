#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QProcess>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    params(new Params(this))
{
    this->setStyleSheet("background-color: rgb(21, 160, 221);");  // 浅紫色（plum）
    ui->setupUi(this);
    ui->openGLWidget->initParams(params);

    //  设置显示模型的初始角度
    ui->horizontalSlider->setValue(50);
    ui->horizontalSlider_2->setValue(90);
    ui->horizontalSlider_3->setValue(0);
    ui->horizontalSlider_4->setValue(300);

    ui->statusBar->showMessage("当前未导入模型文件");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actxc_triggered()
{
    ui->openGLWidget->is_xc = ui->actxc->isChecked();
    update();
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->openGLWidget->scalen = float(value)*0.01f;
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
    ui->horizontalSlider_2->setValue(90);
    ui->horizontalSlider_3->setValue(0);
    ui->horizontalSlider_4->setValue(300);
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
        ui->openGLWidget->slice_situation.clear();
        update();
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

void MainWindow::on_colorshow_triggered()
{
    ui->openGLWidget->is_draw = ui-> colorshow ->isChecked();
    update();
}


void MainWindow::on_action_show_location_triggered()
{
    ui->openGLWidget->is_show_location = ui->action_show_location ->isChecked();
    update();
}

void MainWindow::on_action_show_tick_triggered()
{
    ui->openGLWidget->show_tick = ui->action_show_tick ->isChecked();
    update();
}

void MainWindow::on_action_show_slice_location_triggered()
{
    ui->openGLWidget->show_slice_location = ui->action_show_slice_location ->isChecked();
    update();
}

void MainWindow::on_CreateModel_triggered()
{
    // 生成填写参数的界面
    // 只生成一次
    // 未调用的时候隐藏
    if (setParams == nullptr) {
        setParams = new DialogSetParams(this);
    }

    // 调用窗口填写参数
    bool ready = false; // 标记参数是否正确得填写好
    while (true) {
        // 用户取消填写
        if (setParams->exec() == QDialog::Rejected) {
            qDebug() << "取消填写参数，直接退出";
            break;
        }

        // 读取参数
        params->inputFileName = setParams->inputFile();
        params->outputFileName = setParams->outputFile();
        for (int i = 0; i < 3; i++) {
            params->axisMin[i] = setParams->axisMin(i);
            params->axisStep[i] = setParams->axisStep(i);
            params->axisCount[i] = setParams->axisCount(i);
        }
        params->interpPower = setParams->ipow();
        params->radius = setParams->radius();
        params->minPoint = setParams->minPoints();

        // 如果能正确生成 config 文件就直接退出，否则说明参数填写有误需要重新填写
        if (params->writeConfig()) {
            qDebug() << "正确参数，直接退出";
            ready = true;
            break;
        }
        qDebug() << "填写参数有误，重新填写";
    }

    // 未填写参数就直接返回
    if (not ready) {
        return;
    }

    // 尝试差值程序
    QString programPath = QCoreApplication::applicationDirPath() + "/1DIPL/1DIPL.exe";
    if (!QFileInfo::exists(programPath)) {
        qWarning() << "1DIPL.exe 不存在：" << programPath;
        return;
    }

    QProcess process;
    process.setProgram(programPath);
    process.setWorkingDirectory(QCoreApplication::applicationDirPath() + "/1DIPL");
    process.start();

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

    if (exitCode != 0) {    // 正常退出才开始读取文件
        qDebug() << "差值程序执行错误";
        return;
    }

    qDebug() << "正常退出";
}

void MainWindow::on_LoadModel_triggered()
{
    //bool MainWindow::readFile(const QString &FileName)
    //{
    //    qDebug() << "reading File\n";
    //    // 以只读方式打开数据文件
    //    QFile dataFile(FileName);
    //    qDebug() << "ready to read " << FileName << '\n';
    //    if (not dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) { // QIODevice::Text 处理换行符号
    //        qDebug() << "Fail to read File " << FileName << '\n';
    //        return false;
    //    }
    //    QTextStream in(&dataFile);
    //    QStringList lines = in.readAll()                                // 读取所有行
    //            .split('\n', QString::SkipEmptyParts);    // 只将非空行存到lines里面


    //    ui->openGLWidget->resetData();

    //    unsigned pos = 0; // 记录坐标
    //    bool headLine = true; // 特殊处理第一行
    //    for (const QString &line : lines) {
    //        QStringList valueList = line.split(' ', QString::SkipEmptyParts);
    //        if (valueList.empty()) {
    //            continue;
    //        }

    //        if (headLine) {                         // 第一行读入最值
    //            params->setValueRange(valueList[0].toFloat(), valueList[1].toFloat());
    //            headLine = false;
    //        }
    //        else {
    //            for (const QString &v : valueList) {
    //                ui->openGLWidget->getData(pos++, v.toFloat());
    //            }
    //        }
    //    }
    //    qDebug() << "VPR file read ok !\n" << ui->openGLWidget->modelData.size() << " datas Read\n";
    //    dataFile.close();

    //    QFile wellFile(params->inputFileName);
    //    qDebug() << "ready to read " << params->inputFileName << '\n';
    //    if (not wellFile.open(QIODevice::ReadOnly | QIODevice::Text)) { // QIODevice::Text 处理换行符号
    //        qDebug() << "Fail to read File " << params->inputFileName << '\n';
    //        return false;
    //    }
    //    QTextStream wellIn(&wellFile);
    //    QStringList wellLines = wellIn.readAll()                                // 读取所有行
    //            .split('\n', QString::SkipEmptyParts);    // 只将非空行存到wellLines里面

    //    headLine = true;
    //    for (const QString &line : wellLines) {
    //        QStringList valueList = line.split('\t', QString::SkipEmptyParts);
    //        if (valueList.empty()) {
    //            continue;
    //        }

    //        float z = valueList[3].toFloat(), val = valueList[0].toFloat();
    //        if (params->inRange(2, z)) {
    //            ui->openGLWidget->well.push_back({ z, val });
    //        }

    //        if (headLine) {
    //            ui->openGLWidget->well_x = valueList[1].toFloat();
    //            ui->openGLWidget->well_y = valueList[2].toFloat();
    //            headLine = false;
    //        }
    //    }
    //    qDebug() << "Well file read ok !\n" << ui->openGLWidget->well.size() << " datas Read\n";
    //    qDebug() << ui->openGLWidget->well_x << ' ' << ui->openGLWidget->well_y;
    //    qDebug() << ui->openGLWidget->well.back().first << '\n';
    //    wellFile.close();

    //    ui->openGLWidget->processData();
    //    return true;;
    //}
}
