#include "tagsdialog.h"
#include "ui_tagsdialog.h"
#include <QLabel>
#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>


TagsDialog::TagsDialog(QHexEdit * hexedit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagsDialog)
{
    ui->setupUi(this);
    hexEdit = hexedit;
    bytePattern = new BytePattern(hexEdit);
    edittagDialog = new EditTagDialog(bytePattern,this);
    edittagDialog->hide();
    connect(edittagDialog,SIGNAL(tagReady()),this,SLOT(triggerUpdate()));
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Name"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Color"));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Offset"));
    ui->tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Size"));
    ui->tableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("Type"));
    ui->tableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("Value"));
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    hexEdit->colorTag = &(bytePattern->colorTag);


}

QString TagsDialog::binToStr(QByteArray bin)
{
    QString outString;
    for(int i=0;i<bin.length();i++)
    {
        if(bin.at(i)<32 || uchar(bin.at(i))>=127)
        {
            outString += QString(".");
        }
        else if(bin.at(i) == '<')
        {
            outString += QString("&lt;");
        }
        else if(bin.at(i) == '>')
        {
            outString += QString("&gt;");
        }
        else if(bin.at(i) == '&')
        {
            outString += QString("&amp;");
        }
        else
        {
            outString += QString(bin.at(i));
        }
    }
    return outString;
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
    edittagDialog->colorGen();
}
void TagsDialog::triggerUpdate()
{

    hexEdit->ensureVisible();
    updateTable();
}
void TagsDialog::updateTable()
{
    while(ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }
    //ui->tableWidget->clearContents();

    for(int i=0; i< bytePattern->colorTag.length();i++)
    {
        ColorTag tmpTag = bytePattern->colorTag.at(i);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QString::fromStdString(tmpTag.name)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(" "));
        ui->tableWidget->item(ui->tableWidget->rowCount()-1,1)->setBackground(QColor(QString::fromStdString(tmpTag.color)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,new QTableWidgetItem(QString::fromStdString(tmpTag.obj.offset)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,3,new QTableWidgetItem(QString::fromStdString(tmpTag.obj.size)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,4,new QTableWidgetItem(QString::fromStdString(tagStrings[tmpTag.type])));
        QByteArray baValue = tmpTag.data;
        if(baValue.length() > 8 && (tmpTag.type == LE_t || tmpTag.type == BE_t))
        {
            baValue.truncate(8);
        }
        else if(baValue.length() > 16)
        {
            baValue.truncate(16);
        }
        if(tmpTag.type == HEX_t)
        {
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem(QString(baValue.toHex()).toUpper()));
        }
        else if(tmpTag.type == STRING_t)
        {
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem(binToStr(baValue)));
        }
        else if(tmpTag.type == LE_t)
        {
            bool ok;
            std::reverse(baValue.begin(),baValue.end());
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem(QString("%1").arg(baValue.toHex().toInt(&ok,16))));
            if(!ok)
            {
                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem("Error"));
            }
        }
        else if(tmpTag.type == BE_t)
        {
            bool ok;

            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem(QString("%1").arg(baValue.toHex().toInt(&ok,16))));
            if(!ok)
            {
                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem("Error"));
            }
        }

    }

}


void TagsDialog::on_loadBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Tag selection","patterns","YML (*.yml)");
    if (fileName.isEmpty())
        return;

    bytePattern->ymlParser.loadFile(fileName.toStdString());
    bytePattern->colorTag.clear();
    for(int i=0;i<bytePattern->ymlParser.ymlobj.size();i++)
    {

        int ret =bytePattern->addElement(bytePattern->ymlParser.ymlobj.at(i));
        if(ret)
        {
            QString error_string;
            switch(ret)
                {
                case E_WRONGNAME:
                error_string = QString("Wrong name passed at element %1").arg(i);
                break;
                case E_WRONGTYPE:
                error_string = QString("Wrong type passed at element %1").arg(i);
                break;
                case E_WRONGREF:
                error_string = QString("Wrong reference passed at element %1").arg(i);
                break;
                case E_WRONGCOLOR:
                error_string = QString("Wrong color passed at element %1").arg(i);
                break;
                case E_DUPNAME:
                error_string = QString("Duplicated name passed at element %1").arg(i);
                break;
                case E_EMPTYOFFSET:
                error_string = QString("Empty offset passed at element %1").arg(i);
                break;
                case E_EMPTYSIZE:
                error_string = QString("Empty size passed at element %1").arg(i);
                break;
                default:
                error_string = QString("Error code %1 at element %2").arg(ret).arg(i);
                }

                QMessageBox::warning(this, tr("HexWalk"),
                                     error_string
                                         );
                return;
            }
        }




    updateTable();
    hexEdit->update();
    hexEdit->ensureVisible();

    return;
}


void TagsDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    hexEdit->indexOf("",bytePattern->colorTag.at(index.row()).pos,false,false);
    hexEdit->setSelection(bytePattern->colorTag.at(index.row()).pos*2 + bytePattern->colorTag.at(index.row()).size*2);
    hexEdit->setCursorPosition(bytePattern->colorTag.at(index.row()).pos*2);
    hexEdit->ensureVisible();
}


void TagsDialog::on_saveBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save To YML File"));
    if(!fileName.isEmpty())
    {
        bytePattern->ymlParser.ymlobj.clear();
        for(int i=0;i<bytePattern->colorTag.length();i++)
        {
            bytePattern->ymlParser.ymlobj.push_back(bytePattern->colorTag.at(i).obj);
        }
        bytePattern->ymlParser.saveFile(fileName.toStdString());
        QMessageBox::information(this, tr("HexWalk"),
                             tr("Tag file saved successfully")
                             );
    }

}


void TagsDialog::on_delBtn_clicked()
{
    QModelIndexList selection = ui->tableWidget->selectionModel()->selectedRows();

    // Multiple rows can be selected
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        bytePattern->delElement(index.row());
        ui->tableWidget->removeRow(index.row());
        hexEdit->update();
        hexEdit->ensureVisible();

    }
}


void TagsDialog::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    edittagDialog->show();
    edittagDialog->loadTag(index.row());
}


void TagsDialog::on_resetBtn_clicked()
{
    bytePattern->reset();
    updateTable();
    hexEdit->update();
    hexEdit->ensureVisible();
}
