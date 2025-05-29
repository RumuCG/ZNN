#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "params.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actxc_triggered();

    void on_horizontalSlider_valueChanged(int value); //缩放滑杆

    void on_horizontalSlider_2_valueChanged(int value);  // 下面三个分别是 xyz的旋转滑杆

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_recover_button_clicked();

    void on_DisplayMode_currentIndexChanged(int index);

    void on_PostionSlider_valueChanged(int value);

    void on_colorshow_triggered();

    void on_doubleSpinBox_X_Min_editingFinished();

    void on_doubleSpinBox_Y_Min_editingFinished();

    void on_doubleSpinBox_X_Step_editingFinished();

    void on_spinBox_X_Count_editingFinished();

    void on_doubleSpinBox_Y_Step_editingFinished();

    void on_spinBox_Y_Count_editingFinished();

    void on_doubleSpinBox_Z_Min_editingFinished();

    void on_doubleSpinBox_Z_Step_editingFinished();

    void on_spinBox_Z_Count_editingFinished();

    void on_openFile_triggered();

    void on_drawModel_triggered();

    void on_doubleSpinBox_interp_power_editingFinished();

    void on_doubleSpinBox_radius_editingFinished();

    void on_spinBox_min_points_editingFinished();

    void on_action_show_location_triggered();

    void on_action_show_tick_triggered();

    void on_action_show_slice_location_triggered();

private:
    Ui::MainWindow *ui;
    Params *params;

    bool fileRead; // 标志是否读入了文件
    bool readFile(const QString &FileName);
};

#endif // MAINWINDOW_H
