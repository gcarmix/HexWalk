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
