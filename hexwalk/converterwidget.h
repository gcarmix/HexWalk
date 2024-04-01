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
    ~ConverterWidget();

private slots:

    void on_decTextEdit_textChanged(const QString &arg1);
    void on_hexTextEdit_textChanged(const QString &arg1);
    void on_binTextEdit_textChanged(const QString &arg1);
private:
    Ui::ConverterWidget *ui;
    QString binValue;
    QString decValue;
    QString hexValue;
};

#endif // CONVERTERWIDGET_H
