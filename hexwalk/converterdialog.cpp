#include "converterdialog.h"
#include "ui_converterdialog.h"

ConverterDialog::ConverterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConverterDialog)
{
    ui->setupUi(this);
    binValue = QString("0");
    hexValue = QString("0");
    decValue = QString("0");
    ui->hexTextEdit->setText(hexValue);
    ui->decTextEdit->setText(decValue);
    ui->binTextEdit->setText(binValue);
}

ConverterDialog::~ConverterDialog()
{
    delete ui;
}

void ConverterDialog::on_decTextEdit_textChanged()
{
    if(decValue != ui->decTextEdit->text())
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


}


void ConverterDialog::on_hexTextEdit_textChanged()
{

   if(hexValue != ui->hexTextEdit->text())
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
                decValue = QString("%1").arg(hexValue.toLongLong(NULL,16),1,10);
                binValue = QString("%1").arg(hexValue.toLongLong(NULL,16),1,2);
                blockSignals(true);
                ui->decTextEdit->setText(decValue);
                ui->binTextEdit->setText(binValue);
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

}


void ConverterDialog::on_binTextEdit_textChanged()
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

