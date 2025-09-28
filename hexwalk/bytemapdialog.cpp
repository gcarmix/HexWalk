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
#include "bytemapdialog.h"
#include "ui_bytemapdialog.h"
ByteMapDialog::ByteMapDialog(QHexEdit * hexedit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ByteMapDialog)
{
    ui->setupUi(this);
    _hexedit = hexedit;
    connect(ui->byteMap,SIGNAL(mouseEvent()),this,SLOT(updatePos()));
    connect(ui->byteMap,SIGNAL(mousePress()),this,SLOT(gotoAddress()));
    connect(parent,SIGNAL(fileLoaded()),this,SLOT(refresh()));
    ui->byteMap->setBytesPerLine(256);
    ui->spinCols->setValue(256);
    ui->byteMap->colored = ui->colorBox->isChecked();
}

ByteMapDialog::~ByteMapDialog()
{
    delete ui;
}

void ByteMapDialog::showByteMapDialog()
{
    ui->byteMap->loadBytes(_hexedit);
    this->show();
    this->update();

}

void ByteMapDialog::refresh()
{
    ui->byteMap->loadBytes(_hexedit);

}


void ByteMapDialog::updatePos()
{
    if(ui->byteMap->getCurrentPosition() < _hexedit->getSize())
    {
        ui->edtAddress->setText(QString::asprintf("%02llX (%lld)",ui->byteMap->getCurrentPosition(),ui->byteMap->getCurrentPosition()));
        ui->edtValue->setText(QString::asprintf("%02X (%u)",(unsigned char)(_hexedit->dataAt(ui->byteMap->getCurrentPosition(),1).at(0)),(unsigned char)(_hexedit->dataAt(ui->byteMap->getCurrentPosition(),1).at(0))));
    }

}

void ByteMapDialog::gotoAddress()
{
    _hexedit->setCursorPosition(2*ui->byteMap->getCurrentPosition());
    _hexedit->ensureVisible();
}


void ByteMapDialog::on_spinCols_valueChanged(int arg1)
{
    ui->byteMap->setBytesPerLine(arg1);
}



void ByteMapDialog::on_closeBtn_clicked()
{
    this->hide();
}


void ByteMapDialog::on_colorBox_stateChanged(int arg1)
{

        ui->byteMap->colored = ui->colorBox->isChecked();

}


void ByteMapDialog::on_spinPixels_valueChanged(int arg1)
{
        ui->byteMap->setPixelSize(arg1);
}

