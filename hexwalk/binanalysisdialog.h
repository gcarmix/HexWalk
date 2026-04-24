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
#include <QProcess>
#include <QSettings>
#include "resultType.h"
#include "../qhexedit/qhexedit.h"

namespace Ui {
class binanalysisdialog;
}

class HexdigWorker;

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

    qint64 offsetAtRow(int row) const;

private:
    QList<qint64> tm_offset;
    QList<QString> tm_cursor;
    QList<QString> tm_content;
};

class HexdigTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    HexdigTableModel(QObject *parent = nullptr);

    struct Row {
        qint64 offset;
        QString offsetStr;
        QString type;
        QString size;
        QString info;
    };

    void populateRows(const QList<Row> &rows);
    void clearData();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    qint64 offsetAtRow(int row) const;

private:
    QList<Row> m_rows;
};


class binanalysisdialog : public QDialog
{
    Q_OBJECT

public:
    explicit binanalysisdialog(QHexEdit *hexEdit, QSettings *appSettings, QWidget *parent = nullptr);
    ~binanalysisdialog();
    void analyze(QString filename);

     QHexEdit *_hexEdit;

private slots:
    void on_resultTableView_clicked(const QModelIndex &index);

    void on_closeBtn_clicked();
    void kill_process();
    void renderBinwalkScan(int status_code);
    void renderBinwalkExtract(int status_code);
    void onHexdigFinished();
    void onHexdigFailed(const QString &message);
    void on_extractAllBtn_clicked();

private:
    enum Analyzer { AnalyzerBinwalk, AnalyzerHexdig };
    Analyzer currentAnalyzer() const;
    void startBinwalkScan(const QString &filename);
    void startHexdigScan(const QString &filename);
    void startHexdigExtraction();
    QString hexdigExtractionDir() const;

    Ui::binanalysisdialog *ui;

    QSettings *appSettings = nullptr;
    Analyzer activeAnalyzer = AnalyzerHexdig;

    // Binwalk
    QList<BinwalkResult_S> binwalkResults;
    BinTableModel *binwalkModel = nullptr;
    QProcess *binwalkProcess = nullptr;
    int processType = 0;

    // HexDig
    HexdigTableModel *hexdigModel = nullptr;
    HexdigWorker *hexdigWorker = nullptr;
    bool hexdigExtractMode = false;

    QString curFile;
    QProgressDialog *progrDialog;
};

#endif // BINANALYSISDIALOG_H