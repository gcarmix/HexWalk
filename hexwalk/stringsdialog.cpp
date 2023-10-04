#include "stringsdialog.h"
#include "ui_stringsdialog.h"
#include <QMessageBox>
StringsDialog::StringsDialog(QHexEdit *hexEdit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StringsDialog)
{
    ui->setupUi(this);
    _hexEdit = hexEdit;
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Address"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("String"));
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget->horizontalHeader()->resizeSection(1,400);
}


StringsDialog::~StringsDialog()
{
    delete ui;
}
void StringsDialog::searchStrings()
{

    QStringList stringList;
    qint64 cursor = 0;
    if(!ui->cbBegin->isChecked())
    {
        cursor = _hexEdit->cursorPosition()/2;
    }

    int occurrencies = 0;
    qint64 dataSize = 1024;
    std::string rawString = "";
    bool toAdd = false;
    bool validChar = false;
    QElapsedTimer timer;
    timer.start();

    qInfo()<<"Starting search...\n";
    progrDialog = new QProgressDialog("Search in progress...","Cancel",0,100,this);
    progrDialog->setValue(0);
    progrDialog->show();
    while(ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }
    while(cursor < _hexEdit->getSize())
    {
        dataSize = 4096;
        if(dataSize > (_hexEdit->getSize()-cursor))
        {
            dataSize = (_hexEdit->getSize()-cursor);
        }
        QByteArray data = _hexEdit->dataAt(cursor,dataSize);
        if(ui->cbFindFormat->currentIndex() == 1)
        {
            for(int i=0;i<data.size();i++)
            {
                toAdd = false;
                char test_c = data.at(i);
                if(test_c == 0 && validChar == false)
                {
                    validChar = true;
                    continue;
                }
                if(validChar == true)
                {
                    validChar = false;
                    if(test_c <= 32 || test_c > 126)
                    {
                        toAdd = false;
                    }
                    else if(test_c ==' ' && ui->cbSpaces->isChecked())
                    {
                        toAdd = true;
                    }
                    else if(test_c>='a' && test_c <='z' && ui->cbLower->isChecked())
                    {
                        toAdd = true;
                    }
                    else if(test_c >='A' && test_c <='Z' && ui->cbUpper->isChecked())
                    {
                        toAdd = true;
                    }
                    else if(test_c >='0' && test_c <='9' && ui->cbNumbers->isChecked())
                    {
                        toAdd = true;
                    }
                    else if(ui->cbSymbols->isChecked())
                    {
                        toAdd = true;
                    }
                    if(toAdd == true && (int)rawString.length() < ui->spinBoxMaxLen->value())
                    {
                        rawString.push_back(test_c);
                    }
                    else
                    {
                        if((int)rawString.length() >= ui->spinBoxMinLen->value())
                        {
                            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QString("%1").arg(cursor+i-rawString.length(),1,16).toUpper()));
                            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(QString::fromStdString(rawString)));
                            //stringList.append(QString::fromStdString(rawString));
                            occurrencies++;

                        }
                        rawString.clear();
                    }
                }
            }

        }
        else
        {
            for(int i=0;i<data.size();i++)
            {
                toAdd = false;
                char test_c = data.at(i);
                if(test_c < 32 || test_c > 126)
                {
                    toAdd = false;
                }
                else if(test_c ==' ' && ui->cbSpaces->isChecked())
                {
                    toAdd = true;
                }
                else if(test_c>='a' && test_c <='z' && ui->cbLower->isChecked())
                {
                    toAdd = true;
                }
                else if(test_c >='A' && test_c <='Z' && ui->cbUpper->isChecked())
                {
                    toAdd = true;
                }
                else if(test_c >='0' && test_c <='9' && ui->cbNumbers->isChecked())
                {
                    toAdd = true;
                }
                else if(ui->cbSymbols->isChecked())
                {
                    toAdd = true;
                }
                if(toAdd == true && (int)rawString.length() < ui->spinBoxMaxLen->value())
                {
                    rawString.push_back(test_c);
                }
                else
                {
                    if((int)rawString.length() >= ui->spinBoxMinLen->value())
                    {
                        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QString("%1").arg(cursor+i-rawString.length(),1,16).toUpper()));
                        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(QString::fromStdString(rawString)));
                        //stringList.append(QString::fromStdString(rawString));
                        occurrencies++;

                    }
                    rawString.clear();
                }
            }
        }
        cursor+=dataSize;
        progrDialog->setValue(int(100.0*(double(cursor)/double(_hexEdit->getSize()))));
        QCoreApplication::processEvents();
        if(occurrencies > 65535)
        {

            QMessageBox::warning(this, tr("HexWalk"),tr("Too much occurrencies found, stopping search."));
            break;
        }
        if(progrDialog->wasCanceled())
            break;
    }
    /*for(int i = 0; i < stringList.size() ; i++)
    {
        qInfo()<<i<<" "<<stringList.at(i)<<"\n";
    }*/
    progrDialog->cancel();
    if((int)rawString.length() >= ui->spinBoxMinLen->value())
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        if(ui->cbFindFormat == 0)
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QString("%1").arg(_hexEdit->getSize()-rawString.length(),1,16).toUpper()));
        else
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QString("%1").arg(_hexEdit->getSize()-2*rawString.length(),1,16).toUpper()));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(QString::fromStdString(rawString)));
        //stringList.append(QString::fromStdString(rawString));
        occurrencies++;

    }

    QString message = QString("%1 occurrencies found").arg(occurrencies);
    QMessageBox::information(this, tr("HexWalk"),message);
    qDebug() << timer.elapsed();
    qInfo()<<"done.\n";
}
void StringsDialog::on_pbSearch_clicked()
{
    searchStrings();

}


void StringsDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    _hexEdit->indexOf("",ui->tableWidget->item(index.row(),0)->text().toInt(NULL,16),false,false);
    _hexEdit->setSelection(ui->tableWidget->item(index.row(),0)->text().toInt(NULL,16)*2+ui->tableWidget->item(index.row(),1)->text().length()*2);
    _hexEdit->setCursorPosition(ui->tableWidget->item(index.row(),0)->text().toInt(NULL,16)*2);
    _hexEdit->ensureVisible();

}

