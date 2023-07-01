#include "edittagdialog.h"
#include "ui_edittagdialog.h"
#include <QColorDialog>
EditTagDialog::EditTagDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTagDialog)
{
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

