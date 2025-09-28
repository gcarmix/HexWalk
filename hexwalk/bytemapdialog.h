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
#ifndef BYTEMAPDIALOG_H
#define BYTEMAPDIALOG_H

#include <QDialog>
#include "../qhexedit/qhexedit.h"
#include <QGraphicsScene>
namespace Ui {
class ByteMapDialog;
}

class ByteMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ByteMapDialog(QHexEdit * hexedit,QWidget *parent = nullptr);
    ~ByteMapDialog();
    void showByteMapDialog();

private slots:
    void updatePos();
    void gotoAddress();
    void refresh();
    void on_spinCols_valueChanged(int arg1);
    void on_closeBtn_clicked();

    void on_colorBox_stateChanged(int arg1);

    void on_spinPixels_valueChanged(int arg1);

private:
    int imageWidth;
    int imageHeight;
    Ui::ByteMapDialog *ui;
    QHexEdit * _hexedit;
    QGraphicsScene *scene;
};

#endif // BYTEMAPDIALOG_H
