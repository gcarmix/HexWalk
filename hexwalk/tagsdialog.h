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
#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <QDialog>
#include "edittagdialog.h"
#include "../qhexedit/qhexedit.h"
#include "../qhexedit/tagparser.hpp"
#include <iostream>
#include <stack>
#include <string>
#include "../qhexedit/bytepattern.h"
namespace Ui {
class TagsDialog;
}

class TagsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagsDialog(QHexEdit * hexedit,QWidget *parent = nullptr);
    void changeColor();
    ~TagsDialog();
    void updateTable();
    bool isOperator(char c);
    int performOperation(char operation, int operand1, int operand2);
    int evaluateExpression(const string& expression);
    void analyzePattern();
    BytePattern *bytePattern;
    QString curFileTag;

private slots:
    void on_addBtn_clicked();
    void triggerUpdate();
    void triggerFileUpdate();

    void on_loadBtn_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

    void on_saveBtn_clicked();

    void on_delBtn_clicked();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_resetBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::TagsDialog *ui;
    EditTagDialog * edittagDialog;
    QHexEdit * hexEdit;

    QString binToStr(QByteArray bin);
    void substVariables(string line,int idx);
};

#endif // TAGSDIALOG_H
