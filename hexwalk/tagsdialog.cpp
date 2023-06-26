#include "tagsdialog.h"
#include "ui_tagsdialog.h"

TagsDialog::TagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagsDialog)
{
    ui->setupUi(this);
}

TagsDialog::~TagsDialog()
{
    delete ui;
}
