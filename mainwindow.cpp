#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->openGLWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actdrawrect_triggered()
{
    //  znnwidget::drawshape();
    ui->openGLWidget->drawshape(znnwidget::Rect); // 调用接口
}

void MainWindow::on_actclear_triggered()
{
    ui->openGLWidget->drawshape(znnwidget::None);
}

void MainWindow::on_actframe_triggered()
{
    ui->openGLWidget->frameline(ui->actframe->isChecked());
}

void MainWindow::on_actdy_triggered()
{
    ui->openGLWidget->dy(ui->actdy-> isChecked());
}
