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
#ifndef HASHDIALOG_H
#define HASHDIALOG_H

#include <QDialog>

namespace Ui {
class HashDialog;
}

class HashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HashDialog(QWidget *parent = nullptr);
    ~HashDialog();
    void calculate(QString curFile);
protected:
    void keyPressEvent(QKeyEvent *event);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::HashDialog *ui;
    bool haltCalc;
};

#endif // HASHDIALOG_H
