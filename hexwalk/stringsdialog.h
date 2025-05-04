#ifndef STRINGSDIALOG_H
#define STRINGSDIALOG_H

#include <QDialog>
#include <QProgressDialog>
#include "../qhexedit/qhexedit.h"
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

    void on_btnNext_clicked();

private:
    Ui::StringsDialog *ui;
    QHexEdit * _hexEdit;
    void searchStrings();
    QProgressDialog *progrDialog;
    bool findStringInColumn(const QString& target);

};

#endif // STRINGSDIALOG_H
