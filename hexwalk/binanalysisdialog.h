#ifndef BINANALYSISDIALOG_H
#define BINANALYSISDIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include "resultType.h"
#include "../src/qhexedit.h"

namespace Ui {
class binanalysisdialog;
}

class BinTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    BinTableModel(QObject *parent = 0);

    void populateData(const QList<BinwalkResult_S> &resultsData);
    void clearData();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<QString> tm_cursor;
    QList<QString> tm_content;

};


class binanalysisdialog : public QDialog
{
    Q_OBJECT

public:
    explicit binanalysisdialog(QHexEdit *hexEdit,QWidget *parent = nullptr);
    ~binanalysisdialog();
    void analyze(QString filename);
     QHexEdit *_hexEdit;

private slots:
    void on_binwalkTableView_clicked(const QModelIndex &index);

    void on_closeBtn_clicked();

    void on_extractAllBtn_clicked();

private:
    Ui::binanalysisdialog *ui;
    QList<BinwalkResult_S> resultslist;
    BinTableModel *model = NULL;
    QString curFile;
};

#endif // BINANALYSISDIALOG_H
