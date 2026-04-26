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
#include <QProgressDialog>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

#include "hexdigworker.h"
#include "helpers.hpp"


BinTableModel::BinTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}


void BinTableModel::populateData(const QList<BinwalkResult_S> &resultlist)
{
    beginResetModel();
    tm_offset.clear();
    tm_cursor.clear();
    tm_content.clear();

    for (int i = 0; i < resultlist.count(); i++) {
        tm_offset.append(resultlist.at(i).cursor);
        tm_cursor.append(QString("0x%1").arg(resultlist.at(i).cursor, 0, 16));
        tm_content.append(resultlist.at(i).datastr);
    }
    endResetModel();
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
    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        return static_cast<qlonglong>(tm_offset.value(index.row(), -1));
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.column() == 0)
        return tm_cursor[index.row()];
    if (index.column() == 1)
        return tm_content[index.row()];

    return QVariant();
}

QVariant BinTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0)
            return QString("Offset");
        if (section == 1)
            return QString("Description");
    }
    return QVariant();
}


void BinTableModel::clearData()
{
    beginResetModel();
    tm_offset.clear();
    tm_cursor.clear();
    tm_content.clear();
    endResetModel();
}

qint64 BinTableModel::offsetAtRow(int row) const
{
    return tm_offset.value(row, -1);
}


// -------------------- HexdigTableModel --------------------

HexdigTableModel::HexdigTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void HexdigTableModel::populateRows(const QList<Row> &rows)
{
    beginResetModel();
    m_rows = rows;
    endResetModel();
}

void HexdigTableModel::clearData()
{
    beginResetModel();
    m_rows.clear();
    endResetModel();
}

int HexdigTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_rows.size();
}

int HexdigTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant HexdigTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        return static_cast<qlonglong>(offsetAtRow(index.row()));
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    const Row &r = m_rows.at(index.row());
    switch (index.column()) {
    case 0: return r.offsetStr;
    case 1: return r.type;
    case 2: return r.size;
    case 3: return r.info;
    }
    return QVariant();
}

QVariant HexdigTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return QString("Offset");
        case 1: return QString("Type");
        case 2: return QString("Size");
        case 3: return QString("Info");
        }
    }
    return QVariant();
}

qint64 HexdigTableModel::offsetAtRow(int row) const
{
    if (row < 0 || row >= m_rows.size())
        return -1;
    return m_rows.at(row).offset;
}


// -------------------- binanalysisdialog --------------------

binanalysisdialog::binanalysisdialog(QHexEdit *hexEdit, QSettings *appSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::binanalysisdialog)
{
    ui->setupUi(this);
    _hexEdit = hexEdit;
    this->appSettings = appSettings;

    progrDialog = new QProgressDialog(this);
    progrDialog->setLabelText("Please wait...");
    progrDialog->setModal(true);
    progrDialog->setRange(0, 0);
    progrDialog->setMinimumDuration(500);
    progrDialog->cancel();
    connect(progrDialog, &QProgressDialog::canceled, this, &binanalysisdialog::kill_process);

    binwalkProcess = new QProcess(this);

#ifndef Q_OS_WIN
    // Make sure the common unix install paths are visible to QProcess when
    // searching for the binwalk binary.
    QString path = qgetenv("PATH");
    path = path + ":/usr/local/bin:/opt/homebrew/bin:/opt/local/bin";
    qputenv("PATH", path.toUtf8());
#endif

    connect(binwalkProcess,
            qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this,
            [this](int code, QProcess::ExitStatus) {
                if (processType == 0)
                    renderBinwalkScan(code);
                else
                    renderBinwalkExtract(code);
            });

    connect(ui->resultTableView, &QTableView::clicked,
            this, &binanalysisdialog::on_resultTableView_clicked);
}

binanalysisdialog::~binanalysisdialog()
{
    if (hexdigWorker) {
        if (hexdigWorker->isRunning())
            hexdigWorker->wait();
    }
    delete ui;
}

binanalysisdialog::Analyzer binanalysisdialog::currentAnalyzer() const
{
    QString choice = appSettings
        ? appSettings->value("DefaultAnalyzer", "hexdig").toString()
        : QStringLiteral("hexdig");
    return choice.compare("binwalk", Qt::CaseInsensitive) == 0
        ? AnalyzerBinwalk
        : AnalyzerHexdig;
}

void binanalysisdialog::kill_process()
{
    if (activeAnalyzer == AnalyzerBinwalk) {
        binwalkProcess->kill();
    } else {
        // Scanner::scan has no cancellation hook — the worker will finish on
        // its own. Just hide the dialog so the UI isn't blocked.
        progrDialog->hide();
    }
}

void binanalysisdialog::analyze(QString filename)
{
    curFile = filename;
    activeAnalyzer = currentAnalyzer();

    ui->lblAnalyzer->setText(
        activeAnalyzer == AnalyzerBinwalk
            ? tr("Analyzer: binwalk")
            : tr("Analyzer: HexDig"));

    if (binwalkModel) { delete binwalkModel; binwalkModel = nullptr; }
    if (hexdigModel) { delete hexdigModel; hexdigModel = nullptr; }
    ui->resultTableView->setModel(nullptr);

    progrDialog->show();

    if (activeAnalyzer == AnalyzerBinwalk)
        startBinwalkScan(filename);
    else
        startHexdigScan(filename);
}

// -------------------- Binwalk path (binwalk 3 on PATH) --------------------

void binanalysisdialog::startBinwalkScan(const QString &filename)
{
    processType = 0;
    QStringList params;
    params << filename;
    binwalkProcess->start("binwalk", params);
    binwalkProcess->waitForStarted(2000);
    if (binwalkProcess->state() != QProcess::Running) {
        binwalkProcess->close();
        QMessageBox::warning(this, tr("HexWalk"),
            tr("Could not start binwalk.\r\nError: \r\n%1")
                .arg(tr("binwalk executable not found on PATH (requires binwalk 3).")));
        progrDialog->hide();
    }
}

void binanalysisdialog::renderBinwalkScan(int status_code)
{
    if (status_code != 0) {
        progrDialog->hide();
        QMessageBox::warning(this, tr("HexWalk"),
            tr("Could not start binwalk.\r\nError: \r\n%1")
                .arg(QString(binwalkProcess->readAllStandardError())));
        return;
    }

    binwalkResults.clear();
    QString p_stdout = binwalkProcess->readAll();
    progrDialog->hide();

    QStringList lines = p_stdout.split("\n");

    // Binwalk 3's preamble is longer and variable than binwalk 2's. Find the
    // '---' rule that separates the header from the data rows and start parsing
    // after it; if absent, fall back to accepting any line that starts with a
    // decimal integer.
    int startIdx = -1;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines.at(i).trimmed().startsWith("---")) {
            startIdx = i + 1;
            break;
        }
    }
    if (startIdx < 0)
        startIdx = 0;

    for (int i = startIdx; i < lines.size(); ++i) {
        QString line = lines.at(i);
        if (line.trimmed().isEmpty())
            continue;

        // Expected columns: <decimal> <hex> <description...>
        QString hexStr = line.section(' ', 1, 1, QString::SectionSkipEmpty);
        if (hexStr.startsWith("0x") || hexStr.startsWith("0X"))
            hexStr = hexStr.mid(2);

        bool ok = false;
        qint64 offset = hexStr.toLongLong(&ok, 16);
        if (!ok)
            continue;

        BinwalkResult_S r;
        r.cursor = offset;
        r.datastr = line.section(' ', 2, -1, QString::SectionSkipEmpty);
        binwalkResults.append(r);
    }

    if (binwalkModel) {
        delete binwalkModel;
        binwalkModel = nullptr;
    }
    binwalkModel = new BinTableModel(this);
    binwalkModel->populateData(binwalkResults);

    ui->resultTableView->setModel(binwalkModel);
    ui->resultTableView->setColumnWidth(0, 100);
    ui->resultTableView->setColumnWidth(1, 750);
    ui->resultTableView->resizeRowsToContents();
    ui->resultTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void binanalysisdialog::renderBinwalkExtract(int status_code)
{
    progrDialog->hide();
    if (status_code == 0) {
        const QString outDir = hexdigExtractionDir();
        QMessageBox::information(this, tr("HexWalk"),
            tr("Extraction complete. Output: %1").arg(outDir));
        QDesktopServices::openUrl(QUrl::fromLocalFile(outDir));
    } else {
        QMessageBox::warning(this, tr("HexWalk"),
            tr("Could not start binwalk.\r\nError: \r\n%1")
                .arg(QString(binwalkProcess->readAllStandardError())));
    }
}

// -------------------- HexDig path (in-process library) --------------------

QString binanalysisdialog::hexdigExtractionDir() const
{
    QDir d = QFileInfo(curFile).absoluteDir();
    return d.absolutePath() + "/extracted";
}

void binanalysisdialog::startHexdigScan(const QString &filename)
{
    if (hexdigWorker) {
        if (hexdigWorker->isRunning())
            hexdigWorker->wait();
        hexdigWorker->deleteLater();
        hexdigWorker = nullptr;
    }

    hexdigExtractMode = false;
    hexdigWorker = new HexdigWorker(filename,
                                    /*enableExtraction*/ false,
                                    /*recursionDepth*/ 0,
                                    /*extractionPath*/ QString(),
                                    /*verbose*/ true,
                                    this);
    connect(hexdigWorker, &HexdigWorker::scanFinished,
            this, &binanalysisdialog::onHexdigFinished);
    connect(hexdigWorker, &HexdigWorker::scanFailed,
            this, &binanalysisdialog::onHexdigFailed);
    hexdigWorker->start();
}

void binanalysisdialog::startHexdigExtraction()
{
    if (hexdigWorker) {
        if (hexdigWorker->isRunning())
            hexdigWorker->wait();
        hexdigWorker->deleteLater();
        hexdigWorker = nullptr;
    }

    hexdigExtractMode = true;
    progrDialog->show();
    hexdigWorker = new HexdigWorker(curFile,
                                    /*enableExtraction*/ true,
                                    /*recursionDepth*/ 8,
                                    /*extractionPath*/ hexdigExtractionDir(),
                                    /*verbose*/ false,
                                    this);
    connect(hexdigWorker, &HexdigWorker::scanFinished,
            this, &binanalysisdialog::onHexdigFinished);
    connect(hexdigWorker, &HexdigWorker::scanFailed,
            this, &binanalysisdialog::onHexdigFailed);
    hexdigWorker->start();
}

// Flatten a ScanResult tree into table rows in DFS order, preserving the
// top-level offset so row clicks can still navigate the hex view.
static void flattenHexdigResults(const std::vector<ScanResult> &results,
                                 int depth,
                                 qint64 rootOffset,
                                 bool isRoot,
                                 QList<HexdigTableModel::Row> &out)
{
    for (const auto &res : results) {
        HexdigTableModel::Row row;
        row.offset = isRoot ? static_cast<qint64>(res.offset) : rootOffset;
        QString indent = QString(depth * 2, QChar(' '));
        row.offsetStr = indent + QString("0x%1").arg(
            static_cast<qlonglong>(res.offset), 0, 16);
        row.type = QString::fromStdString(res.type);
        row.size = QString::number(static_cast<qulonglong>(res.length));
        row.info = QString::fromStdString(res.info);
        out.append(row);

        if (!res.children.empty()) {
            flattenHexdigResults(res.children,
                                 depth + 1,
                                 row.offset,
                                 /*isRoot*/ false,
                                 out);
        }
    }
}

void binanalysisdialog::onHexdigFinished()
{
    progrDialog->hide();

    if (hexdigExtractMode) {
        const QString outDir = hexdigExtractionDir();
        QMessageBox::information(this, tr("HexWalk"),
            tr("Extraction complete. Output: %1").arg(outDir));
        QDesktopServices::openUrl(QUrl::fromLocalFile(outDir));
    }

    if (!hexdigWorker)
        return;

    QList<HexdigTableModel::Row> rows;
    flattenHexdigResults(hexdigWorker->results, 0, 0, true, rows);

    if (hexdigModel) {
        delete hexdigModel;
        hexdigModel = nullptr;
    }
    hexdigModel = new HexdigTableModel(this);
    hexdigModel->populateRows(rows);

    ui->resultTableView->setModel(hexdigModel);
    ui->resultTableView->setColumnWidth(0, 120);
    ui->resultTableView->setColumnWidth(1, 120);
    ui->resultTableView->setColumnWidth(2, 100);
    ui->resultTableView->setColumnWidth(3, 520);
    ui->resultTableView->resizeRowsToContents();
    ui->resultTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void binanalysisdialog::onHexdigFailed(const QString &message)
{
    progrDialog->hide();
    QMessageBox::warning(this, tr("HexWalk"),
        tr("HexDig analysis failed.\r\nError: \r\n%1").arg(message));
}

// -------------------- UI slots --------------------

void binanalysisdialog::on_resultTableView_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QAbstractItemModel *m = ui->resultTableView->model();
    if (!m)
        return;

    QVariant v = m->data(m->index(index.row(), 0), Qt::UserRole);
    bool ok = false;
    qint64 offset = v.toLongLong(&ok);
    if (!ok || offset < 0)
        return;

    _hexEdit->indexOf("", offset, false, false);
    _hexEdit->setCursorPosition(offset * 2);
    _hexEdit->update();
}


void binanalysisdialog::on_closeBtn_clicked()
{
    this->close();
}


void binanalysisdialog::on_extractAllBtn_clicked()
{
    if (activeAnalyzer == AnalyzerBinwalk) {
        processType = 1;
        progrDialog->show();

        QStringList params;
        QDir d = QFileInfo(curFile).absoluteDir();
        QString curDir = d.absolutePath() + "/extracted";
        params << "-e" << curFile << "-C" << curDir;
        binwalkProcess->start("binwalk", params);
        binwalkProcess->waitForStarted(2000);
        if (binwalkProcess->state() != QProcess::Running) {
            binwalkProcess->close();
            QMessageBox::warning(this, tr("HexWalk"),
                tr("Could not start binwalk.\r\nError: \r\n%1")
                    .arg(tr("binwalk executable not found on PATH (requires binwalk 3).")));
            progrDialog->hide();
        }
    } else {
        if (!is_7z_available()) {
            QMessageBox::warning(this, tr("HexWalk"),
                tr("7-Zip was not found.\n\n"
                   "Some hexdig extractors (7z, deb, ...) need the 7z command-line "
                   "tool to unpack archived data. Without it, those formats will be "
                   "skipped during extraction.\n\n"
                   "Install 7-Zip from https://www.7-zip.org/ and make sure it is on "
                   "your PATH, or place 7zr.exe / 7zz next to hexwalk."));
        }
        startHexdigExtraction();
    }
}