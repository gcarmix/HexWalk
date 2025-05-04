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
#include "diffdialog.h"
#include "ui_diffdialog.h"
#include <QScrollBar>
#include <QProgressDialog>

DiffDialog::DiffDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiffDialog)
{
    ui->setupUi(this);
}

DiffDialog::~DiffDialog()
{
    delete ui;
}

void DiffDialog::trackCursor1()
{
    int scrollpos = ui->hexEdit1->verticalScrollBar()->value();
    ui->hexEdit2->verticalScrollBar()->setValue(scrollpos);
}
void DiffDialog::trackCursor2()
{
    int scrollpos = ui->hexEdit2->verticalScrollBar()->value();
    ui->hexEdit1->verticalScrollBar()->setValue(scrollpos);
}
void DiffDialog::setFiles(QString file1str, QString file2str)
{
    QSettings settings;

    ui->hexEdit1->setAddressArea(settings.value("AddressArea").toBool());
    ui->hexEdit1->setAsciiArea(settings.value("AsciiArea").toBool());
    ui->hexEdit1->setHighlighting(settings.value("Highlighting").toBool());
    ui->hexEdit1->setOverwriteMode(settings.value("OverwriteMode").toBool());
    ui->hexEdit1->setReadOnly(settings.value("ReadOnly").toBool());

    ui->hexEdit1->setHighlightingColor(settings.value("HighlightingColor").value<QColor>());
    ui->hexEdit1->setAddressAreaColor(settings.value("AddressAreaColor").value<QColor>());
    ui->hexEdit1->setSelectionColor(settings.value("SelectionColor").value<QColor>());
    ui->hexEdit1->setFont(settings.value("WidgetFont").value<QFont>());
    ui->hexEdit1->setAddressFontColor(settings.value("AddressFontColor").value<QColor>());
    ui->hexEdit1->setAsciiAreaColor(settings.value("AsciiAreaColor").value<QColor>());
    ui->hexEdit1->setAsciiFontColor(settings.value("AsciiFontColor").value<QColor>());
    ui->hexEdit1->setHexFontColor(settings.value("HexFontColor").value<QColor>());

    ui->hexEdit1->setAddressWidth(settings.value("AddressAreaWidth").toInt());
    ui->hexEdit1->setBytesPerLine(settings.value("BytesPerLine").toInt());
    ui->hexEdit1->setHexCaps(settings.value("HexCaps", true).toBool());

    ui->hexEdit2->setAddressArea(settings.value("AddressArea").toBool());
    ui->hexEdit2->setAsciiArea(settings.value("AsciiArea").toBool());
    ui->hexEdit2->setHighlighting(settings.value("Highlighting").toBool());
    ui->hexEdit2->setOverwriteMode(settings.value("OverwriteMode").toBool());
    ui->hexEdit2->setReadOnly(settings.value("ReadOnly").toBool());

    ui->hexEdit2->setHighlightingColor(settings.value("HighlightingColor").value<QColor>());
    ui->hexEdit2->setAddressAreaColor(settings.value("AddressAreaColor").value<QColor>());
    ui->hexEdit2->setSelectionColor(settings.value("SelectionColor").value<QColor>());
    ui->hexEdit2->setFont(settings.value("WidgetFont").value<QFont>());
    ui->hexEdit2->setAddressFontColor(settings.value("AddressFontColor").value<QColor>());
    ui->hexEdit2->setAsciiAreaColor(settings.value("AsciiAreaColor").value<QColor>());
    ui->hexEdit2->setAsciiFontColor(settings.value("AsciiFontColor").value<QColor>());
    ui->hexEdit2->setHexFontColor(settings.value("HexFontColor").value<QColor>());

    ui->hexEdit2->setAddressWidth(settings.value("AddressAreaWidth").toInt());
    ui->hexEdit2->setBytesPerLine(settings.value("BytesPerLine").toInt());
    ui->hexEdit2->setHexCaps(settings.value("HexCaps", true).toBool());


    file1.setFileName(file1str);
    file2.setFileName(file2str);

    ui->hexEdit1->setData(file1);
    ui->hexEdit2->setData(file2);
    connect(ui->hexEdit1,SIGNAL(currentAddressChanged(qint64)),this,SLOT(trackCursor1(qint64)));
    connect(ui->hexEdit2,SIGNAL(currentAddressChanged(qint64)),this,SLOT(trackCursor2(qint64)));

}

void DiffDialog::on_nextDiffBtn_clicked()
{
    progrDialog = new QProgressDialog("Task in progress...","Cancel",0,100,this);
    progrDialog->setValue(0);
    progrDialog->setModal(true);
    progrDialog->show();
    qint64 pos = ui->hexEdit1->cursorPosition();
    progrDialog->setValue(100.0*(double)(pos/2)/ui->hexEdit1->getSize());
    int idx = 0;
    while(pos/2 < ui->hexEdit1->getSize())
    {
        QByteArray data1 = ui->hexEdit1->dataAt(pos/2,1024);
        QByteArray data2 = ui->hexEdit2->dataAt(pos/2,1024);
        if(!data1.compare(data2))
        {
            pos+=2*1024;
        }
        else
        {
            if(ui->hexEdit2->dataAt(pos/2,1)!=ui->hexEdit1->dataAt(pos/2,1))
            {
                ui->hexEdit2->setCursorPosition(pos);
                ui->hexEdit1->ensureVisible();
                ui->hexEdit2->ensureVisible();
                if(idx != 0)
                    break;
            }
            pos+=2;
        }
        ui->hexEdit1->setCursorPosition(pos);
        idx++;


        if(progrDialog->wasCanceled())
        {
            break;
        }
        progrDialog->setValue((int)(100.0*(double)(pos/2)/ui->hexEdit1->getSize()));
        QCoreApplication::processEvents();

    }
    progrDialog->hide();

}


void DiffDialog::on_exitBtn_clicked()
{
    this->hide();
}


void DiffDialog::on_prevDiffBtn_clicked()
{
    qint64 pos = ui->hexEdit1->cursorPosition();
    int idx = 0;
    while(pos > 0)
    {
        if(ui->hexEdit2->dataAt(pos/2,1)!=ui->hexEdit1->dataAt(pos/2,1))
        {
            ui->hexEdit2->setCursorPosition(pos);
            ui->hexEdit1->ensureVisible();
            ui->hexEdit2->ensureVisible();
            if(idx != 0)
                break;
        }
        pos-=2;
        ui->hexEdit1->setCursorPosition(pos);
        idx++;
    }
}

