#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <QDialog>
#include "edittagdialog.h"
namespace Ui {
class TagsDialog;
}

class TagsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagsDialog(QWidget *parent = nullptr);
    void changeColor();
    ~TagsDialog();

private slots:
    void on_addBtn_clicked();

private:
    Ui::TagsDialog *ui;
    EditTagDialog * edittagDialog;
};

#endif // TAGSDIALOG_H
