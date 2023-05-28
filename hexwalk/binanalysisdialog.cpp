#include "binanalysisdialog.h"
#include "ui_binanalysisdialog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProgressDialog>
#include <QMessageBox>


BinTableModel::BinTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}


void BinTableModel::populateData(const QList<BinwalkResult_S> &resultlist)
{

    tm_cursor.clear();
    tm_content.clear();


    for(int i=0;i<resultlist.count();i++)
    {
        QString cursor;
        cursor = QString("%1").arg(resultlist.at(i).cursor,4,16);
        tm_cursor.append(cursor);
        tm_content.append(resultlist.at(i).datastr);

    }




    return;
}


int BinTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return tm_cursor.length();
}

int BinTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant BinTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return tm_cursor[index.row()];
    } else if (index.column() == 1) {
        return tm_content[index.row()];
    }

    return QVariant();
}

QVariant BinTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Cursor");
        }else if (section == 1) {
            return QString("Content");
        }
    }
    return QVariant();
}


void BinTableModel::clearData()
{
    tm_cursor.clear();
    tm_content.clear();
}

binanalysisdialog::binanalysisdialog(QHexEdit *hexEdit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::binanalysisdialog)
{
    ui->setupUi(this);
    _hexEdit = hexEdit;
}

binanalysisdialog::~binanalysisdialog()
{
    delete ui;
}


void binanalysisdialog::analyze(QString filename)
{
    QCoreApplication::processEvents();
    curFile = filename;
    BinwalkResult_S binwalkResult;
    resultslist.clear();
    QThread::msleep(200);
    QCoreApplication::processEvents();
    QProgressDialog progrDialog("Wait while binwalk is processing...","Cancel",0,0,this);
    progrDialog.setModal(true);
    progrDialog.setRange(0,0);
    progrDialog.show();

    progrDialog.setValue(0);
    QCoreApplication::processEvents();
    QProcess p;
    QStringList params;


#ifdef Q_OS_WIN
    params << "binw.py" << filename;
    p.start("python",params);
#else
    params << filename;
    p.start("binwalk",params);
#endif

    p.waitForFinished(-1);

    QString p_stdout = p.readAll();
    progrDialog.cancel();
    QStringList lines = p_stdout.split("\n");
    if(lines.length() < 3)
    {
        return;
    }
    lines.removeFirst();
    lines.removeFirst();
    lines.removeFirst();
    QString line;
    foreach(line,lines)
    {
        if(line.length()>0){
            binwalkResult.cursor = line.section(' ',1,1,QString::SectionSkipEmpty).toLong(NULL,16);
            binwalkResult.datastr = line.section(' ',2,-1,QString::SectionSkipEmpty);

            resultslist.append(binwalkResult);
        }


    }



    if(model)
    {
        delete model;
    }
    model = new BinTableModel(this);

    model->populateData(resultslist);

    ui->binwalkTableView->setModel(model);
    ui->binwalkTableView->setColumnWidth(1,750);
ui->binwalkTableView->resizeRowsToContents();
    ui->binwalkTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

}

void binanalysisdialog::on_binwalkTableView_clicked(const QModelIndex &index)
{
    _hexEdit->indexOf("",resultslist.at(index.row()).cursor,false,false);
    _hexEdit->setCursorPosition(resultslist.at(index.row()).cursor*2);
    _hexEdit->update();
}


void binanalysisdialog::on_closeBtn_clicked()
{
    this->close();
}


void binanalysisdialog::on_extractAllBtn_clicked()
{

    QProgressDialog progrDialog("Wait while binwalk is extracting data...","Cancel",0,0,this);
    progrDialog.setModal(true);
    progrDialog.setRange(0,0);
    progrDialog.show();
    QApplication::processEvents();
    QProcess p;
    QStringList params;

#ifdef Q_OS_WIN
    params << "binw.py"<<"-e" << curFile;
    p.start("python",params);
#else
    params << "-e" << curFile;
    p.start("binwalk",params);
#endif

    p.waitForFinished(-1);
    QString p_stdout = p.readAll();
    progrDialog.cancel();
}

