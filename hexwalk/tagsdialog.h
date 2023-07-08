#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <QDialog>
#include "edittagdialog.h"
#include "../src/qhexedit.h"
#include "tagparser.hpp"
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

    void on_loadBtn_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

    void on_saveBtn_clicked();

    void on_delBtn_clicked();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_resetBtn_clicked();

private:
    Ui::TagsDialog *ui;
    EditTagDialog * edittagDialog;
    QHexEdit * hexEdit;
    YMLParser * ymlParser;
    QString binToStr(QByteArray bin);
};

#endif // TAGSDIALOG_H
