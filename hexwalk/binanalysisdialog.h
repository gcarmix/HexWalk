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
#ifndef BINANALYSISDIALOG_H
#define BINANALYSISDIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include <QProgressDialog>
#include "resultType.h"
#include "../qhexedit/qhexedit.h"

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
    void kill_process();
    void renderAnalysis(int status_code);
    void on_extractAllBtn_clicked();

private:
    Ui::binanalysisdialog *ui;
    QList<BinwalkResult_S> resultslist;
    BinTableModel *model = NULL;
    QString curFile;
    QProgressDialog * progrDialog;
    QProcess * binwalkProcess;
    int processType = 0;
};

#endif // BINANALYSISDIALOG_H
