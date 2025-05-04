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
#include "edittagdialog.h"
#include "ui_edittagdialog.h"
#include <QColorDialog>
#include "../qhexedit/colortag.h"
#include <QMessageBox>
EditTagDialog::EditTagDialog(BytePattern * bytePtr,QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::EditTagDialog)
{
    bytePattern = bytePtr;
    ui->setupUi(this);


}

EditTagDialog::~EditTagDialog()
{
    delete ui;
}

void EditTagDialog::colorGen()
{
    QRandomGenerator prng(time(0));

    QColor color(prng.generate()%256,prng.generate()%256,prng.generate()%256);
    if(color.isValid()) {
        QString qss = QString("background-color: %1").arg(color.name());
        ui->colorButton->setStyleSheet(qss);
    }
    editedTagIdx = -1;
}

void EditTagDialog::loadTag(int row)
{
    ColorTag tag = bytePattern->colorTag.at(row);
    QString qss = QString("background-color: %1").arg(QString::fromStdString(tag.color));
    ui->colorButton->setStyleSheet(qss);
    ui->nameEdit->setText(QString::fromStdString(tag.name));
    ui->offsetEdit->setText(QString::fromStdString(tag.obj.offset));
    ui->lengthEdit->setText(QString::fromStdString(tag.obj.size));
    if(tag.type == BE_t)
    {
        ui->typeCombo->setCurrentIndex(BE_t-1);
    }
    else if(tag.type == LE_t)
    {
        ui->typeCombo->setCurrentIndex(LE_t-1);
    }
    else if(tag.type == STRING_t)
    {
        ui->typeCombo->setCurrentIndex(STRING_t-1);
    }
    else
    {
        ui->typeCombo->setCurrentIndex(HEX_t-1);
    }
    editedTagIdx = row;
}

void EditTagDialog::on_colorButton_clicked()
{
    QColor color = QColorDialog::getColor();
    if(color.isValid()) {
        QString qss = QString("background-color: %1").arg(color.name());
        ui->colorButton->setStyleSheet(qss);
    }
}

void EditTagDialog::on_applyButton_clicked()
{
    YMLObj obj;
    BPerrors error = E_NONE;
    obj.color = ui->colorButton->palette().button().color().name().toStdString();
    obj.name = ui->nameEdit->text().toStdString();
    obj.offset = ui->offsetEdit->text().toStdString();
    obj.size = ui->lengthEdit->text().toStdString();
    obj.type = tagStrings[ui->typeCombo->currentIndex() +1 ];

    if(!bytePattern->isValidColor(obj.color))
    {
        error = E_WRONGCOLOR;
    }
    else if(!bytePattern->isValidName(obj.name))
    {
        error = E_WRONGNAME;
    }
    else if(bytePattern->isDuplicatedName(obj.name,editedTagIdx))
    {
        error = E_DUPNAME;
    }
    else if(obj.offset.length() == 0)
    {
        error = E_EMPTYOFFSET;
    }
    else if(obj.size.length() == 0)
    {
        error = E_EMPTYSIZE;
    }
    QString error_string;
    if(error != E_NONE)
    {
    switch(error)
    {
    case E_WRONGNAME:
        error_string = QString("Wrong name passed ");
        break;
    case E_WRONGTYPE:
        error_string = QString("Wrong type passed ");
        break;
    case E_WRONGREF:
        error_string = QString("Wrong reference passed" );
        break;
    case E_WRONGCOLOR:
        error_string = QString("Wrong color passed");
        break;
    case E_DUPNAME:
        error_string = QString("Duplicated name");
        break;
    default:
        error_string = QString("Error code %1").arg(error);
    }

    QMessageBox::warning(this, tr("HexWalk"),
                         error_string
                         );
    }
    else
    {

        if(editedTagIdx >= 0)
        {
            bytePattern->updateElement(editedTagIdx,obj);
            editedTagIdx = -1;
        }
        else {
            bytePattern->addElement(obj);
        }

        emit tagReady();
        this->hide();
    }


}

