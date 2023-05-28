#ifndef HASHDIALOG_H
#define HASHDIALOG_H

#include <QDialog>

namespace Ui {
class HashDialog;
}

class HashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HashDialog(QWidget *parent = nullptr);
    ~HashDialog();
    void calculate(QString curFile);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::HashDialog *ui;
    bool haltCalc;
};

#endif // HASHDIALOG_H
