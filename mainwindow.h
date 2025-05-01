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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
