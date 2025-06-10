#ifndef DIALOGSETPARAMS_H
#define DIALOGSETPARAMS_H

#include <QDialog>
#include "baseparams.h"

namespace Ui {
class DialogSetParams;
}

class DialogSetParams : public QDialog, public BaseParams
{
    Q_OBJECT

public:
    explicit DialogSetParams(QWidget *parent = nullptr);
    ~DialogSetParams();

    QString inputFile;
    QString outputFile;

    // IPL信息
    float interpPower;
    float radius;
    int minPoints;

    void getParams();
    bool writeConfig();              // 写数据到 config.ini
    QString getExe();
private slots:
    void on_BtnChooseInput_clicked();

    void on_BtnSaveAs_clicked();

private:
    Ui::DialogSetParams *ui;
    static constexpr long long LimitScale = 1e10; // 1e10
    bool chkData();                   // 检查参数的合法性

    const QString DefaultOutFile = "未指定路径（默认同上面选择的文件同路径）";
};

#endif // DIALOGSETPARAMS_H
