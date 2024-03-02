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
    setFixedWidth(48+256*3);
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
    if(arg1 > 512)
    {
      setFixedWidth(32+arg1*2);
    }
    else
    {
    setFixedWidth(48+arg1*3);
    }
}



void ByteMapDialog::on_closeBtn_clicked()
{
    this->hide();
}


void ByteMapDialog::on_colorBox_stateChanged(int arg1)
{

        ui->byteMap->colored = ui->colorBox->isChecked();

}

