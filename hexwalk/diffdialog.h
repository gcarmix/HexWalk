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
    void trackCursor1();
    void trackCursor2();

    void on_nextDiffBtn_clicked();

    void on_exitBtn_clicked();

    void on_prevDiffBtn_clicked();

private:
    Ui::DiffDialog *ui;
    QProgressDialog *progrDialog;
};

#endif // DIFFDIALOG_H
