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
    hexEdit->colorTag.append(tag);
    hexEdit->ensureVisible();
    emit tagReady();
    this->hide();


}

