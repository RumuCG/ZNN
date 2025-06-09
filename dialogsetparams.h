#ifndef DIALOGSETPARAMS_H
#define DIALOGSETPARAMS_H

#include <QDialog>

namespace Ui {
class DialogSetParams;
}

class DialogSetParams : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetParams(QWidget *parent = nullptr);
    ~DialogSetParams();

    QString inputFile();
    QString outputFile();

    float axisMin(int id);
    float axisStep(int id);
    int axisCount(int id);

    float ipow();
    float radius();
    int minPoints();

private slots:
    void on_BtnChooseInput_clicked();

    void on_BtnSaveAs_clicked();

private:
    Ui::DialogSetParams *ui;
};

#endif // DIALOGSETPARAMS_H
