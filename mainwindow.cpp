#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
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

    switch (index) {
    case 0:             // NULL 啥都不显示

        ui->openGLWidget->setShape(ui->openGLWidget->None);
        ui->openGLWidget->update();

        break;
    case 1:             // A 显示全部 6 个面

        ui->openGLWidget->setShape(ui->openGLWidget->Surface);
        ui->openGLWidget->getSurfaceIndex();

        break;
    case 2:             // X 沿X轴显示切片

        ui->openGLWidget->setShape(ui->openGLWidget->SliceYZ);
        ui->openGLWidget->getSliceIndex(2, 0u);

        break;
    case 3:             // Y 沿Y轴显示切片

        ui->openGLWidget->setShape(ui->openGLWidget->SliceXZ);
        ui->openGLWidget->getSliceIndex(3, 0u);

        break;
    case 4:             // Z 沿Z轴显示切片

        ui->openGLWidget->setShape(ui->openGLWidget->SliceXY);
        ui->openGLWidget->getSliceIndex(4, 0u);

        break;
    default:
        break;
    }
}
