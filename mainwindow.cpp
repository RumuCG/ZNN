#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include "vertexdata.h"
#include <fstream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setStyleSheet("background-color: rgb(21, 160, 221);");  // 浅紫色（plum）
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actdrawrect_triggered()
{
    QString path = QCoreApplication::applicationDirPath() + "/input.bin";
    fileRead = readFile(path);
}

void MainWindow::on_actclear_triggered()
{

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
    ui->PositionBox->setEnabled(enable);

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
        ui->PostionSlider->setMaximum(ui->openGLWidget->num_x - 1);
        qDebug() << ui->openGLWidget->num_x;
        ui->openGLWidget->setShape(ui->openGLWidget->SliceYZ);
        ui->openGLWidget->getSliceIndex(2, 0u);

        break;
    case 3:             // Y 沿Y轴显示切片
        ui->PostionSlider->setValue(0);
        ui->PostionSlider->setMinimum(0);
        ui->PostionSlider->setMaximum(ui->openGLWidget->num_y - 1);
        ui->openGLWidget->setShape(ui->openGLWidget->SliceXZ);
        ui->openGLWidget->getSliceIndex(3, 0u);

        break;
    case 4:             // Z 沿Z轴显示切片
        ui->PostionSlider->setValue(0);
        ui->PostionSlider->setMinimum(0);
        ui->PostionSlider->setMaximum(ui->openGLWidget->num_z - 1);
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
//    qDebug() << "reading File\n";
//    // 以只读方式打开数据文件
//    QFile dataFile(FileName);
//    qDebug() << "ready to read " << FileName << '\n';
//    if (not dataFile.open(QIODevice::ReadOnly)) {
//        qDebug() << "Fail to read File " << FileName << '\n';
//        return false;
//    }

    ui->openGLWidget->resetData();

//    QDataStream dataSrc(&dataFile);
//    dataSrc.setVersion(QDataStream::Qt_5_9);
//    dataSrc >> d_min >> d_max; // 开头一行是最小值和最大值
//    qDebug() << d_min << ' ' << d_max << '\n';
//    if (d_min != 3000.0f) {
//        exit(1);
//    }
//    else {
//        qDebug() << "read successfully!\n";
//    }

    std::ifstream dataSrc(FileName.toStdString(), std::ios::binary);
    float d;
    dataSrc.read(reinterpret_cast<char*>(&d), sizeof(float));
    d_min = 4000.0f;
    dataSrc.read(reinterpret_cast<char*>(&d), sizeof(float));
    d_max = 4500.0f;
    if (d != 5000.0f) {
        qDebug() << d << ' ' << "Fail to read\n";
        exit(0);
    }

    // 以下参数都是我们写给配置文件的参数，是已知的
    // 由于还未实现创建配置文件与差值程序交互，这里先用测试文件的参数
    int x = 250, y = 250, z = 175;
    ui->openGLWidget->modelData.reserve(x * y * z);
    float x_min = -5000.0f, y_min = -5000.0f, z_min = 0.0f;
    float x_d = 40.0f, y_d = 40.0f, z_d = 40.0f;
    int id = 0;
    for (int i = 0; i < x; i++) {
        if (i % 50 == 0) {
            qDebug() << i << "-th reading\n";
        }
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < z; k++) {
                dataSrc.read(reinterpret_cast<char*>(&d), sizeof(float));
                VertexData node(x_min + i * x_d, y_min + j * y_d, z_min + k * z_d);
                node.setColor(d, d_min, d_max);
                ui->openGLWidget->modelData.push_back(node);
            }
        }
    }

    qDebug() << "File read ok !\n";
//    exit(1);

    ui->openGLWidget->processData();

//    dataFile.close();
    return true;
}
