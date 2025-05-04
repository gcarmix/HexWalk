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
#ifndef EDITTAGDIALOG_H
#define EDITTAGDIALOG_H

#include <QDialog>
#include "../qhexedit/qhexedit.h"
#include "../qhexedit/bytepattern.h"
namespace Ui {
class EditTagDialog;
}

class EditTagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTagDialog(BytePattern * bytePtr,QWidget *parent = nullptr);
    void colorGen();
    void loadTag(int row);
    ~EditTagDialog();
    BytePattern * bytePattern;

private slots:
    void on_colorButton_clicked();

    void on_applyButton_clicked();
signals:
    void tagReady();
private:
    Ui::EditTagDialog *ui;
    QHexEdit * hexEdit;
    int editedTagIdx;
};

#endif // EDITTAGDIALOG_H
