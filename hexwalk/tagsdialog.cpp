#include "tagsdialog.h"
#include "ui_tagsdialog.h"
#include <QLabel>
#include <QColorDialog>
#include <QDebug>


TagsDialog::TagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagsDialog)
{
    ui->setupUi(this);
    edittagDialog = new EditTagDialog(this);
    edittagDialog->hide();

}
void TagsDialog::changeColor()
{
    QColor color = QColorDialog::getColor();
}
TagsDialog::~TagsDialog()
{
    delete ui;
}

void TagsDialog::on_addBtn_clicked()
{
    edittagDialog->show();
}

