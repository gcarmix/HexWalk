#ifndef CONVERTERDIALOG_H
#define CONVERTERDIALOG_H

#include <QDialog>

namespace Ui {
class ConverterDialog;
}

class ConverterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConverterDialog(QWidget *parent = nullptr);
    ~ConverterDialog();

private slots:
    void on_decTextEdit_textChanged();

    void on_hexTextEdit_textChanged();

    void on_binTextEdit_textChanged();

private:
    Ui::ConverterDialog *ui;
    QString binValue;
    QString decValue;
    QString hexValue;
};

#endif // CONVERTERDIALOG_H
