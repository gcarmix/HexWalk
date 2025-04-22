#ifndef CONVERTERWIDGET_H
#define CONVERTERWIDGET_H

#include <QDockWidget>

namespace Ui {
class ConverterWidget;
}

class ConverterWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ConverterWidget(QWidget *parent = nullptr);
    void update(QString hexString);
    ~ConverterWidget();

private slots:

    void on_decTextEdit_textChanged(const QString &arg1);
    void on_hexTextEdit_textChanged(const QString &arg1);
    void on_binTextEdit_textChanged(const QString &arg1);
    void on_checkBox_be_stateChanged(int arg1);
    //void update(QByteArray ba);

private:
    Ui::ConverterWidget *ui;
    bool isBE=false;
    bool isSigned=false;
    void updateDec();
    void updateHex();
    QString binValue;
    QString decValue;
    QString hexValue;
};

#endif // CONVERTERWIDGET_H
