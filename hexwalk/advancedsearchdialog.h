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
#ifndef ADVANCEDSEARCHDIALOG_H
#define ADVANCEDSEARCHDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QTimer>
#include <QProgressDialog>
#include <QAbstractTableModel>
#include "../qhexedit/qhexedit.h"
#include "resultType.h"

namespace Ui {
    class AdvancedSearchDialog;
}

class FindAllWorker;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModel(QObject *parent = 0);

    void populateData(const QList<Result_S> &resultsData);
    void clearData();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<QString> tm_cursor;
    QList<QString> tm_contenthex;
    QList<QString> tm_content;

};

class AdvancedSearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AdvancedSearchDialog(QHexEdit *hexEdit, QWidget *parent = 0);
    ~AdvancedSearchDialog();
    qint64 findNext();
    void findAll();

    Ui::AdvancedSearchDialog *ui;


    QHexEdit *_hexEdit;
    QProgressDialog * progrDialog;

private slots:
    void on_pbFind_clicked();



    void on_resultsTableView_clicked(const QModelIndex &index);

void on_pbFindAll_clicked();
    void on_pbCancel_clicked();

void on_cbCase_clicked();
    void on_cbInvertMatch_clicked();
    void on_cbFindFormat_currentIndexChanged(int index);

void on_cbBackwards_clicked();

    void on_cbBegin_clicked();

void on_cbRegex_clicked();

private:
    void setData();
    QByteArray getContent(int comboIndex, const QString &input);
    qint64 replaceOccurrence(qint64 idx, const QByteArray &replaceBa);
    QByteArray _findBa;
    QList<Result_S> resultslist;
    TableModel *model = NULL;
    QString binToStr(QByteArray bin);

};



#endif // ADVANCEDSEARCHDIALOG_H
