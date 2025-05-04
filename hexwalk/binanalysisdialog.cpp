/*
 * Copyright (C) 2025 Carmix <carmixdev@gmail.com>
 *
 * This file is part of HexWalk.
 *
 * HexWalk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HexWalk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "binanalysisdialog.h"
#include "ui_binanalysisdialog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QProcessEnvironment>
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
    progrDialog = new QProgressDialog(this);
    progrDialog->setLabelText("Please wait...");
    progrDialog->setModal(true);
    progrDialog->setRange(0,0);
    progrDialog->setMinimumDuration(500);
    progrDialog->cancel();
    connect(progrDialog,SIGNAL(canceled()),this,SLOT(kill_process()));

    binwalkProcess = new QProcess();

    #ifdef Q_OS_WIN
    if(!QFile::exists("binw.py"))
    {
        QFile file( "binw.py" );
        if ( file.open(QIODevice::ReadWrite) )
        {
            QTextStream stream( &file );
            stream << "import sys" << Qt::endl;
            stream << "import os" << Qt::endl;
            stream << "sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'binwalk', 'src'))" << Qt::endl;
            stream << "import binwalk" << Qt::endl;
            stream << "if len(sys.argv) == 2:" << Qt::endl;
            stream << "    binwalk.scan('--signature', sys.argv[1])" << Qt::endl;
            stream << "elif len(sys.argv) == 3 and sys.argv[1] == '-e':" << Qt::endl;
            stream << "    binwalk.scan('--signature','--extract', sys.argv[2])" << Qt::endl;
        }
        file.close();
    }
#else
    QString path = qgetenv("PATH");
    path = path + ":/usr/local/bin:/opt/homebrew/bin:/opt/local/bin";
    qputenv("PATH",path.toUtf8());
    #endif



    connect(binwalkProcess,SIGNAL(finished(int)),this,SLOT(renderAnalysis(int)));
}

binanalysisdialog::~binanalysisdialog()
{
    delete ui;
}

void binanalysisdialog::kill_process()
{
    binwalkProcess->kill();
}

void binanalysisdialog::renderAnalysis(int status_code)
{
    if(processType == 0)
    {
        BinwalkResult_S binwalkResult;

        if(status_code != 0)
        {
            progrDialog->hide();
            QMessageBox::warning(this, tr("HexWalk"),
                                 tr("Could not start binwalk.\r\nError: \r\n%1").arg(QString(binwalkProcess->readAllStandardError())));
            //qInfo() <<QString(binwalkProcess->readAllStandardError());
            return;
        }
        resultslist.clear();
        QString p_stdout = binwalkProcess->readAll();
        progrDialog->hide();
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
                binwalkResult.cursor = line.section(' ',1,1,QString::SectionSkipEmpty).toLongLong(NULL,16);
                binwalkResult.datastr = line.section(' ',2,-1,QString::SectionSkipEmpty);

                resultslist.append(binwalkResult);
            }


        }

        if(model)
        {
            delete model;
            model = NULL;
        }
        model = new BinTableModel(this);

        model->populateData(resultslist);

        ui->binwalkTableView->setModel(model);
        ui->binwalkTableView->setColumnWidth(1,750);
        ui->binwalkTableView->resizeRowsToContents();
        ui->binwalkTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    }
    else
    {
        progrDialog->hide();
        if(status_code == 0)
        {
            QMessageBox::warning(this, tr("HexWalk"),
                                 "Extraction complete.");

        }
        else
        {
            progrDialog->hide();
            QMessageBox::warning(this, tr("HexWalk"),
                                 tr("Could not start binwalk.\r\nError: \r\n%1").arg(QString(binwalkProcess->readAllStandardError())));
            //qInfo() <<QString(binwalkProcess->readAllStandardError());
            return;
        }
    }

}

void binanalysisdialog::analyze(QString filename)
{
    //QCoreApplication::processEvents();
    processType = 0;
    curFile = filename;
    if(model)
    {
        delete model;
        model = NULL;
    }
    progrDialog->show();

    QStringList params;
#ifdef Q_OS_WIN
    params << "binw.py" << filename;
    binwalkProcess->start("py",params);
#else
    params << filename;
    binwalkProcess->start("binwalk",params);
#endif
    binwalkProcess->waitForStarted(2000);
    if(binwalkProcess->state() != QProcess::Running)
    {
        binwalkProcess->close();
#ifdef Q_OS_WIN
        QMessageBox::warning(this, tr("HexWalk"),tr("Could not start binwalk.\r\nError: \r\n%1").arg("Python executable not found"));
#else
        QMessageBox::warning(this, tr("HexWalk"),tr("Could not start binwalk.\r\nError: \r\n%1").arg("Binwalk executable not found"));
#endif
        progrDialog->hide();
    }

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
    processType = 1;

    progrDialog->show();

    QStringList params;

#ifdef Q_OS_WIN
    params << "binw.py"<<"-e" << curFile;
    binwalkProcess->start("py",params);
    if(binwalkProcess->state() != QProcess::Running)
    {
        binwalkProcess->close();
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("Could not start binwalk.\r\nError: \r\n%1").arg("Python executable not found"));
        progrDialog->hide();
    }
#else
    QDir d = QFileInfo(curFile).absoluteDir();
    QString curDir=d.absolutePath();
    curDir=curDir+"/extracted";
    params << "-e" << curFile << "-C"<<curDir;
    binwalkProcess->start("binwalk",params);
#endif

}

