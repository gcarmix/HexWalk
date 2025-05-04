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
#include "searchdialog.h"
#include "ui_searchdialog.h"

#include <QMessageBox>

SearchDialog::SearchDialog(QHexEdit *hexEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
  ui->setupUi(this);
  _hexEdit = hexEdit;
}

SearchDialog::~SearchDialog()
{
  delete ui;
}

qint64 SearchDialog::findNext()
{
    qint64 from = _hexEdit->cursorPosition() / 2;
    _findBa = getContent(ui->cbFindFormat->currentIndex(), ui->cbFind->currentText());
    qint64 idx = -1;

    if (_findBa.length() > 0)
    {
        if (ui->cbBackwards->isChecked())
            idx = _hexEdit->lastIndexOf(_findBa, from);
        else
            idx = _hexEdit->indexOf(_findBa, from,false,false);
    }
    return idx;
}

void SearchDialog::on_pbFind_clicked()
{
    findNext();
}

void SearchDialog::on_pbReplace_clicked()
{
    int idx = findNext();
    if (idx >= 0)
    {
        QByteArray replaceBa = getContent(ui->cbReplaceFormat->currentIndex(), ui->cbReplace->currentText());
        replaceOccurrence(idx, replaceBa);
    }
}

void SearchDialog::on_pbReplaceAll_clicked()
{
    int replaceCounter = 0;
    int idx = 0;
    int goOn = QMessageBox::Yes;

    while ((idx >= 0) && (goOn == QMessageBox::Yes))
    {
        idx = findNext();
        if (idx >= 0)
        {
            QByteArray replaceBa = getContent(ui->cbReplaceFormat->currentIndex(), ui->cbReplace->currentText());
            int result = replaceOccurrence(idx, replaceBa);

            if (result == QMessageBox::Yes)
                replaceCounter += 1;

            if (result == QMessageBox::Cancel)
                goOn = result;
        }
    }

    if (replaceCounter > 0)
        QMessageBox::information(this, tr("QHexEdit"), QString(tr("%1 occurrences replaced.")).arg(replaceCounter));
}


QByteArray SearchDialog::getContent(int comboIndex, const QString &input)
{
    QByteArray findBa;
    switch (comboIndex)
    {
        case 0:     // hex
            findBa = QByteArray::fromHex(input.toLatin1());
            break;
        case 1:     // text
            findBa = input.toUtf8();
            break;
    }
    return findBa;
}

qint64 SearchDialog::replaceOccurrence(qint64 idx, const QByteArray &replaceBa)
{
    int result = QMessageBox::Yes;
    if (replaceBa.length() >= 0)
    {
        if (ui->cbPrompt->isChecked())
        {
            result = QMessageBox::question(this, tr("QHexEdit"),
                     tr("Replace occurrence?"),
                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (result == QMessageBox::Yes)
            {
                _hexEdit->replace(idx, replaceBa.length(), replaceBa);
                _hexEdit->update();
            }
        }
        else
        {
            _hexEdit->replace(idx, _findBa.length(), replaceBa);
        }
    }
    return result;
}
