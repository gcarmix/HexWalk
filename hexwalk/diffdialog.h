#ifndef DIFFDIALOG_H
#define DIFFDIALOG_H

#include <QDialog>
#include <QFile>
#include <QProgressDialog>

namespace Ui {
class DiffDialog;
}

class DiffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiffDialog(QWidget *parent = nullptr);
    ~DiffDialog();
    void setFiles(QString file1, QString file2);
    QFile file1;
    QFile file2;
private slots:
    void trackCursor1(qint64 pos);
    void trackCursor2(qint64 pos);

    void on_nextDiffBtn_clicked();

    void on_exitBtn_clicked();

    void on_prevDiffBtn_clicked();

private:
    Ui::DiffDialog *ui;
    QProgressDialog *progrDialog;
};

#endif // DIFFDIALOG_H
