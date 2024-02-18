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
    ui->comboCols->setCurrentIndex(1);
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

void ByteMapDialog::on_buttonBox_accepted()
{
    this->hide();
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

void ByteMapDialog::on_comboCols_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0:
        {
            ui->byteMap->setBytesPerLine(128);
            setFixedWidth(48+128*3);
        }break;
        case 1:
        {
            ui->byteMap->setBytesPerLine(256);
            setFixedWidth(48+256*3);
        }break;
        case 2:
        {
            ui->byteMap->setBytesPerLine(512);
            setFixedWidth(48+512*3);
        }break;


    }


}

