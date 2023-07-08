#ifndef EDITTAGDIALOG_H
#define EDITTAGDIALOG_H

#include <QDialog>
#include "../src/qhexedit.h"
namespace Ui {
class EditTagDialog;
}

class EditTagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTagDialog(QHexEdit * hexedit,QWidget *parent = nullptr);
    void colorGen();
    void loadTag(int row);
    ~EditTagDialog();

private slots:
    void on_pushButton_clicked();

    void on_colorButton_clicked();

    void on_applyButton_clicked();
signals:
    void tagReady();
private:
    Ui::EditTagDialog *ui;
    QHexEdit * hexEdit;
    int editedTagIdx;
};

#endif // EDITTAGDIALOG_H
