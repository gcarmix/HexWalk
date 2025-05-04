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
#include "converterwidget.h"
#include "ui_converterwidget.h"

ConverterWidget::ConverterWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ConverterWidget)
{
    ui->setupUi(this);
    hide();
}

ConverterWidget::~ConverterWidget()
{
    delete ui;
}

void ConverterWidget::on_decTextEdit_textChanged(const QString &arg1)
{
    if(decValue != ui->decTextEdit->text())
    {
        updateDec();
    }

}


void ConverterWidget::on_hexTextEdit_textChanged(const QString &arg1)
{
    if(hexValue != ui->hexTextEdit->text())
    {
        updateHex();
    }

}
void ConverterWidget::update(QString hexString)
{
    ui->hexTextEdit->setText(hexString);
}

void ConverterWidget::on_binTextEdit_textChanged(const QString &arg1)
{
    if(binValue != ui->binTextEdit->text())
    {
        QString check = ui->binTextEdit->text();
        int i;
        bool good = true;
        if(check.length()){
            for(i=0;i<check.length();i++)
            {
                if(check.at(i) != 0x30 && check.at(i) != 0x31)
                {
                    good = false;
                    break;
                }
            }

            if(ui->binTextEdit->text().length() < 64 && good == true)
            {
                binValue = ui->binTextEdit->text();
                decValue = QString("%1").arg(binValue.toLongLong(NULL,2),1,10);
                hexValue = QString("%1").arg(binValue.toLongLong(NULL,2),1,16);
                blockSignals(true);
                ui->decTextEdit->setText(decValue);
                ui->hexTextEdit->setText(hexValue);
                blockSignals(false);
            }
            else
            {
                blockSignals(true);
                ui->binTextEdit->setText(binValue);
                blockSignals(false);
            }
        }
    }
}
void ConverterWidget::updateHex()
{
    QString check = ui->hexTextEdit->text();
    int i;
    bool good = true;
    if(check.length()){
        for(i=0;i<check.length();i++)
        {
            if(check.at(i) < 0x30 || (check.at(i) >0x39 && check.at(i) < 0x41) || (check.at(i)>0x46 && check.at(i) < 0x61) ||(check.at(i)> 0x66))
            {
                good = false;
                break;
            }
        }

        if(ui->hexTextEdit->text().length() <= 16 && good == true)
        {
            hexValue = ui->hexTextEdit->text();
            QByteArray baValue = QByteArray::fromHex(hexValue.toUtf8());

            binValue = QString("%1").arg(hexValue.toLongLong(NULL,16),1,2);
            if(!isBE)
            {
                std::reverse(baValue.begin(),baValue.end());
            }
            decValue= (QString("%1").arg((signed long long)baValue.toHex().toULongLong(NULL,16)));
            //decValue = QString("%1").arg(hexValue.toLongLong(NULL,16),1,10);

            blockSignals(true);
            ui->decTextEdit->setText(decValue);
            ui->binTextEdit->setText(binValue);
            qint64 num = (signed long long)baValue.toHex().toULongLong(NULL,16);
            float *numf;
            numf = (float *)&num;

            ui->floatTextEdit->setText(QString::number(*numf));

            blockSignals(false);
        }
        else
        {
            blockSignals(true);
            ui->hexTextEdit->setText(hexValue);
            blockSignals(false);
        }
    }
}
void ConverterWidget::updateDec()
{
    QString check = ui->decTextEdit->text().replace(" ","");
    int i;
    bool good = true;
    if(check.length())
    {
        for(i=0;i<check.length();i++)
        {
            if(check.at(i) < 0x30 || check.at(i) >0x39)
            {
                good = false;
                break;
            }
        }

        if(ui->decTextEdit->text().length() <= 16 && good == true)
        {
            decValue = ui->decTextEdit->text();
            hexValue = QString("%1").arg(decValue.toLongLong(),1,16);
            binValue = QString("%1").arg(decValue.toLongLong(),1,2);
            blockSignals(true);
            ui->hexTextEdit->setText(hexValue);
            ui->binTextEdit->setText(binValue);
            blockSignals(false);
        }
        else
        {
            blockSignals(true);
            ui->decTextEdit->setText(decValue);
            blockSignals(false);
        }
    }
}


void ConverterWidget::on_checkBox_be_stateChanged(int arg1)
{
    isBE = ui->checkBox_be->isChecked();
    updateHex();

}

