#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <QDialog>
#include "edittagdialog.h"
#include "../src/qhexedit.h"
namespace Ui {
class TagsDialog;
}

class TagsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagsDialog(QHexEdit * hexedit,QWidget *parent = nullptr);
    void changeColor();
    ~TagsDialog();
    void updateTable();

private slots:
    void on_addBtn_clicked();
    void triggerUpdate();

private:
    Ui::TagsDialog *ui;
    EditTagDialog * edittagDialog;
    QHexEdit * hexEdit;
};

#endif // TAGSDIALOG_H
