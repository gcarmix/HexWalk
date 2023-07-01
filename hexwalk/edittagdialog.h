#ifndef EDITTAGDIALOG_H
#define EDITTAGDIALOG_H

#include <QDialog>

namespace Ui {
class EditTagDialog;
}

class EditTagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTagDialog(QWidget *parent = nullptr);
    ~EditTagDialog();

private slots:
    void on_pushButton_clicked();

    void on_colorButton_clicked();

private:
    Ui::EditTagDialog *ui;
};

#endif // EDITTAGDIALOG_H
