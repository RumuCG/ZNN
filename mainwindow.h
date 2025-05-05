#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_actdrawrect_triggered();

    void on_actclear_triggered();

    void on_actframe_triggered();

    void on_actdy_triggered();

    void on_actxc_triggered();


    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_recover_button_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
