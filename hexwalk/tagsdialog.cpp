#include "tagsdialog.h"
#include "ui_tagsdialog.h"
#include <QLabel>
#include <QColorDialog>
#include <QDebug>


TagsDialog::TagsDialog(QHexEdit * hexedit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagsDialog)
{
    ui->setupUi(this);
    hexEdit = hexedit;
    edittagDialog = new EditTagDialog(hexedit,this);
    edittagDialog->hide();
    connect(edittagDialog,SIGNAL(tagReady()),this,SLOT(triggerUpdate()));
    ui->tableWidget->setColumnCount(1+1+1+1+1+1);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Name"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Color"));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Offset"));
    ui->tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Size"));
    ui->tableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("Type"));
    ui->tableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("Value"));
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


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
void TagsDialog::triggerUpdate()
{


    updateTable();
}
void TagsDialog::updateTable()
{
    while(ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }
    //ui->tableWidget->clearContents();

    for(int i=0; i< hexEdit->colorTag.length();i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(hexEdit->colorTag.at(i).name));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(" "));
        ui->tableWidget->item(ui->tableWidget->rowCount()-1,1)->setBackground(QColor(hexEdit->colorTag.at(i).color));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(hexEdit->colorTag.at(i).pos)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,3,new QTableWidgetItem(QString("%1").arg(hexEdit->colorTag.at(i).size)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,4,new QTableWidgetItem(QString("%1").arg(hexEdit->colorTag.at(i).type)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem(QString(hexEdit->dataAt(hexEdit->colorTag.at(i).pos,hexEdit->colorTag.at(i).size).toHex())));
    }

}

