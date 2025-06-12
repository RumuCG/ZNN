#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setStyleSheet("background-color: rgb(21, 160, 221);");  // 浅紫色（plum）
    ui->setupUi(this);
    ui->statusBar->showMessage("当前未读取文件");
    //  设置显示模型的初始角度
    ui->horizontalSlider->setValue(50);
    ui->horizontalSlider_2->setValue(90);
    ui->horizontalSlider_3->setValue(0);
    ui->horizontalSlider_4->setValue(300);
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
        ui->openGLWidget->setShape(ui->openGLWidget->None);
        ui->openGLWidget->update();
    }
    else if (not fileRead) {
        ui->DisplayMode->setCurrentIndex(0);
        index = 0;
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
        ui->PostionSlider->setMaximum(ui->openGLWidget->getMaxCount(0) - 1);
        // qDebug() << ui->openGLWidget->num_x;
        ui->openGLWidget->setShape(ui->openGLWidget->SliceYZ);
        ui->openGLWidget->getSliceIndex(2, 0u);

        break;
    case 3:             // Y 沿Y轴显示切片
        ui->PostionSlider->setValue(0);
        ui->PostionSlider->setMinimum(0);
        ui->PostionSlider->setMaximum(ui->openGLWidget->getMaxCount(1) - 1);
        ui->openGLWidget->setShape(ui->openGLWidget->SliceXZ);
        ui->openGLWidget->getSliceIndex(3, 0u);

        break;
    case 4:             // Z 沿Z轴显示切片
        ui->PostionSlider->setValue(0);
        ui->PostionSlider->setMinimum(0);
        ui->PostionSlider->setMaximum(ui->openGLWidget->getMaxCount(2) - 1);
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

        // 读取用户写入的参数
        setParams->getParams();

        // 如果能正确生成 config 文件就直接退出，否则说明参数填写有误需要重新填写
        if (setParams->writeConfig()) {
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
    QString exe = setParams->getExe();
    QString programPath = QCoreApplication::applicationDirPath() + "/Interpolation/" + exe;
    if (!QFileInfo::exists(programPath)) {
        qWarning() << exe << " 不存在：" << programPath;
        return;
    }
    qDebug() << "Starting " << programPath;

    QProcess process;
    process.setProgram(programPath);
    process.setWorkingDirectory(QCoreApplication::applicationDirPath() + "/Interpolation");
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
    // 选择模型文件
    QString filePath = QFileDialog::getOpenFileName(
                this,
                "选择模型文件",
                QDir::homePath(), // 默认目录
                "模型文件 (*.vpr)" // 过滤器
                );
    // 未选择文件
    if (filePath.isEmpty()) {
        return;
    }

    fileRead = ui->openGLWidget->loadData(filePath);

    return;
}
