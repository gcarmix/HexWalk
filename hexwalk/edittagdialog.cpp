#include "edittagdialog.h"
#include "ui_edittagdialog.h"
#include <QColorDialog>
#include "../src/colortag.h"
#include <QMessageBox>
EditTagDialog::EditTagDialog(QHexEdit * hexedit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTagDialog)
{
    hexEdit = hexedit;
    ui->setupUi(this);


}

EditTagDialog::~EditTagDialog()
{
    delete ui;
}

void EditTagDialog::on_pushButton_clicked()
{ 
    this->hide();
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
    ColorTag tag = hexEdit->colorTag.at(row);
    QString qss = QString("background-color: %1").arg(tag.color);
    ui->colorButton->setStyleSheet(qss);
    ui->nameEdit->setText(tag.name);
    ui->offsetEdit->setText(QString("%1").arg(tag.pos));
    ui->lengthEdit->setText(QString("%1").arg(tag.size));
    if(tag.type == BE_t)
    {
        ui->typeCombo->setCurrentIndex(BE_t);
    }
    else if(tag.type == LE_t)
    {
        ui->typeCombo->setCurrentIndex(LE_t);
    }
    else if(tag.type == STRING_t)
    {
        ui->typeCombo->setCurrentIndex(STRING_t);
    }
    else
    {
        ui->typeCombo->setCurrentIndex(HEX_t);
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
    ColorTag tag;
    tag.color = ui->colorButton->palette().button().color().name();
    tag.name = ui->nameEdit->text();
    tag.pos = QString(ui->offsetEdit->text()).toLong();
    tag.size = QString(ui->lengthEdit->text()).toLong();
    tag.type = TagType_e(ui->typeCombo->currentIndex());
    if(tag.pos < 0)
    {
        QMessageBox::warning(this, "HexWalk",
                             "Position can't be less than 0");
        return;

    }
    if(tag.size < 1)
    {
        QMessageBox::warning(this, "HexWalk",
                             "Size can't be less than 1");
        return;
    }
    if(editedTagIdx >= 0)
    {
        hexEdit->colorTag[editedTagIdx] = tag;
        editedTagIdx = -1;
    }
    else {
       hexEdit->colorTag.append(tag);
    }

    hexEdit->ensureVisible();
    emit tagReady();
    this->hide();


}

