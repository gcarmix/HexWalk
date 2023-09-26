#ifndef STRINGSDIALOG_H
#define STRINGSDIALOG_H

#include <QDialog>
#include <QProgressDialog>
#include "../src/qhexedit.h"
namespace Ui {
class StringsDialog;
}

class StringsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StringsDialog(QHexEdit *hexEdit,QWidget *parent = nullptr);
    ~StringsDialog();

private slots:
    void on_pbSearch_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

private:
    Ui::StringsDialog *ui;
    QHexEdit * _hexEdit;
    void searchStrings();
    QProgressDialog *progrDialog;
};

#endif // STRINGSDIALOG_H
