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
    QString path = QCoreApplication::applicationDirPath() + "/velocity_model.txt";
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

    // 以下参数都是我们写给配置文件的参数，是已知的
    // 由于还未实现创建配置文件与差值程序交互，这里先用测试文件的参数
    int x = 25, y = 25, z = 24;
    ui->openGLWidget->modelData.reserve(x * y * z);
    float x_min = 0.0f, y_min = -500.0f, z_min = 2640.0f;
    float x_d = 40.0f, y_d = 40.0f, z_d = 40.0f;

    int pos = 0; // 记录坐标
    bool headLine = true; // 特殊处理第一行
    for (const QString &line : lines) {
        QStringList valueList = line.split(' ', QString::SkipEmptyParts);
        if (valueList.empty()) {
            continue;
        }

        if (headLine) {                         // 第一行读入最值
            d_min = valueList[0].toFloat();
            d_max = valueList[1].toFloat();
            headLine = false;
            qDebug() << d_min << ' ' << d_max << '\n';
        }
        else {
            for (const QString &v : valueList) {
                // 根据pos 计算坐标 (i, j, k)
                // pos = y * z * i + z * j + k
                int tmp = pos + 1;
                int k = pos % z;
                pos /= z;
                int j = pos % y;
                pos /= y;
                int i = pos;
                pos = tmp;

                VertexData node(x_min + i * x_d, y_min + j * y_d, z_min + k * z_d);
                node.setColor(v.toFloat(), d_min, d_max);
                ui->openGLWidget->modelData.push_back(node);
            }
        }
    }

    qDebug() << "File read ok !\n" << ui->openGLWidget->modelData.size() << " datas Read\n";

    ui->openGLWidget->processData();

    dataFile.close();
    return true;
}
