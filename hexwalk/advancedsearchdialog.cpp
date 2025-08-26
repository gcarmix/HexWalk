/*
 * Copyright (C) 2025 Carmix <carmixdev@gmail.com>
 *
 * This file is part of HexWalk.
 *
 * HexWalk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HexWalk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "advancedsearchdialog.h"
#include "ui_advancedsearchdialog.h"
#include <QDebug>
#include <QMessageBox>
#include "resultType.h"
#include "worditemdelegate.h"
#include <QProgressDialog>
#include <QMessageBox>

AdvancedSearchDialog::AdvancedSearchDialog(QHexEdit *hexEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedSearchDialog)
{
  ui->setupUi(this);
  _hexEdit = hexEdit;
  //model = new TableModel(this);


}

AdvancedSearchDialog::~AdvancedSearchDialog()
{
  delete ui;
}

qint64 AdvancedSearchDialog::findNext()
{
    qint64 from = _hexEdit->cursorPosition() / 2;
    _findBa = getContent(ui->cbFindFormat->currentIndex(), ui->cbFind->currentText());
    qint64 idx = -1;

    if (_findBa.length() > 0)
    {
        if (ui->cbBackwards->isChecked())
            idx = _hexEdit->lastIndexOf(_findBa, from);
        else
            idx = _hexEdit->indexOf(_findBa, from,ui->cbRegex->isChecked(),ui->cbCase->isChecked(), ui->cbInvertMatch->isChecked());
    }
    return idx;
}

void AdvancedSearchDialog::on_pbFind_clicked()
{
    if(ui->cbBegin->isChecked() == true)
    {
        _hexEdit->setCursorPosition(0);
        ui->cbBegin->setChecked(false);
    }
    findNext();

}

QByteArray AdvancedSearchDialog::getContent(int comboIndex, const QString &input)
{
    QByteArray findBa;
    switch (comboIndex)
    {
        case 2:     // hex
            findBa = QByteArray::fromHex(input.toLatin1());
            break;
        case 0:     // text
            findBa = input.toUtf8();
            break;
        case 1:

            findBa = QByteArray::fromRawData((char*)(input.utf16()),input.length()*2);
    }
    return findBa;
}
QString AdvancedSearchDialog::binToStr(QByteArray bin)
{
    QString outString;
    for(int i=0;i<bin.length();i++)
    {
            if(bin.at(i)<32 || uchar(bin.at(i))>127)
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
void AdvancedSearchDialog::findAll()
{
    progrDialog = new QProgressDialog("Task in progress...","Cancel",0,100,this);
    progrDialog->setValue(0);
    progrDialog->setModal(true);
    progrDialog->show();



    resultslist.clear();
    int ret = 1;
    Result_S result;
    int count=0;

    while(ret > 0)
    {
        ret = findNext();

        if(progrDialog->wasCanceled() == true)
        {
                break;
        }
        progrDialog->setValue(int(100.0*double(_hexEdit->cursorPosition()/2)/double(_hexEdit->getSize())));
        QCoreApplication::processEvents();

        if(ret > -1)
        {
                QByteArray preData;
                QByteArray postData;

                result.cursor=ret;
                QByteArray inData = _hexEdit->selectedDataBa();
                if(_hexEdit->cursorPosition()/2 > 8)
                {
                    preData = _hexEdit->dataAt(ret-8,8);
                }
                else
                {
                    preData = _hexEdit->dataAt(ret-(_hexEdit->cursorPosition()/2),_hexEdit->cursorPosition()/2);
                }
                qint64 remainingBytes = _hexEdit->getSize() - _hexEdit->cursorPosition()/2;
                if(remainingBytes > 8)
                {
                    postData = _hexEdit->dataAt(ret+inData.length(),8);
                }
                else {
                   postData = _hexEdit->dataAt(ret+inData.length(),remainingBytes);
                }
                if(resultslist.length() > 0)
                {
                    if(resultslist.last().cursor == result.cursor)
                    {
                       break;
                    }
                }

                result.datastr = binToStr(preData) + QString("<b style='color:red'>")+ binToStr(inData) + QString("</b>")+ binToStr(postData);
                result.hexdatastr = preData.toHex()+ QString("<b style='color:red'>") + inData.toHex() + QString("</b>")+ postData.toHex();
                resultslist.append(result);
                count++;
        }

        if(count > 65535)
        {

                QMessageBox::warning(this, tr("HexWalk"),tr("Too much occurrencies found, stopping search."));
                break;
        }
    }

    QString message = QString("%1 occurrencies found").arg(count);
    QMessageBox::information(this, tr("HexWalk"),message);


    setData();
    ui->resultsTableView->resizeColumnsToContents();
    ui->resultsTableView->verticalHeader()->show();
    ui->resultsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    QCoreApplication::processEvents();

}
void AdvancedSearchDialog::on_pbFindAll_clicked()
{
    if(ui->cbBegin->isChecked() == true)
    {
        _hexEdit->setCursorPosition(0);
    }
    findAll();
}

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
}


void TableModel::populateData(const QList<Result_S> &resultlist)
{
    tm_cursor.clear();
    tm_content.clear();
    tm_contenthex.clear();

    for(int i=0;i<resultlist.count();i++)
    {
        QString cursor;
        cursor = QString("%1").arg(resultlist.at(i).cursor,4,16);
        tm_cursor.append(cursor);
        tm_content.append(resultlist.at(i).datastr);
        tm_contenthex.append(resultlist.at(i).hexdatastr);
    }




    return;
}


int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return tm_cursor.length();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return tm_cursor[index.row()];
    } else if (index.column() == 1) {
        return tm_content[index.row()];
    }else if (index.column() == 2) {
        return tm_contenthex[index.row()];
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Cursor");
        } else if (section == 2) {
            return QString("Content(Hex)");
        }else if (section == 1) {
            return QString("Content");
        }
    }
    return QVariant();
}

void AdvancedSearchDialog::setData()
{
    progrDialog->cancel();

    try{
        if(model)
        {
            delete model;
        }
        model = new TableModel(this);

        model->populateData(resultslist);
        ui->resultsTableView->clearSpans();
        ui->resultsTableView->setModel(model);
        auto wordItemDelegate = new WordItemDelegate(this);
        ui->resultsTableView->setItemDelegate(wordItemDelegate);
    }
    catch(QException &)
    {
        qInfo() <<"Exception!!\n";
    }
}

void AdvancedSearchDialog::on_resultsTableView_clicked(const QModelIndex &index)
{
    //_hexEdit->indexOf("",resultslist.at(index.row()).cursor,ui->cbRegex->isChecked(),ui->cbCase->isChecked(),ui->cbFindFormat->currentIndex()==1);
    _hexEdit->setCursorPosition(resultslist.at(index.row()).cursor);
    _hexEdit->indexOf(_findBa, resultslist.at(index.row()).cursor - 1,ui->cbRegex->isChecked(),ui->cbCase->isChecked(), ui->cbInvertMatch->isChecked());
    _hexEdit->update();

}
void TableModel::clearData()
{
    tm_cursor.clear();
    tm_content.clear();
    tm_contenthex.clear();
}
void AdvancedSearchDialog::on_pbCancel_clicked()
{

    resultslist.clear();


}


void AdvancedSearchDialog::on_cbCase_clicked()
{
    if(ui->cbCase->isChecked())
    {
        ui->cbRegex->setChecked(true);
        ui->cbBackwards->setChecked(false);
    }

}

void AdvancedSearchDialog::on_cbInvertMatch_clicked()
{
    if(ui->cbInvertMatch->isChecked())
    {
        ui->cbBackwards->setChecked(false);
        ui->cbBackwards->setEnabled(false);
    }
    else
    {
        ui->cbBackwards->setChecked(false);
        ui->cbBackwards->setEnabled(true);
    }
}

void AdvancedSearchDialog::on_cbFindFormat_currentIndexChanged(int index)
{
    if(!ui->cbFindFormat->currentText().compare("UTF-8"))
    {
        ui->cbRegex->setEnabled(true);
        ui->cbCase->setEnabled(true);
    }
    else
    {
        ui->cbRegex->setChecked(false);
        ui->cbCase->setChecked(false);
        ui->cbRegex->setEnabled(false);
        ui->cbCase->setEnabled(false);
    }
}


void AdvancedSearchDialog::on_cbBackwards_clicked()
{
    if(ui->cbBackwards->isChecked())
    {
        ui->cbRegex->setChecked(false);
        ui->cbRegex->setEnabled(false);
        ui->cbCase->setChecked(false);
        ui->cbCase->setEnabled(false);
        ui->cbInvertMatch->setChecked(false);
        ui->cbInvertMatch->setEnabled(false);
        ui->cbBegin->setChecked(false);
        ui->cbBegin->setEnabled(false);
    }
    else
    {
        ui->cbRegex->setChecked(false);
        ui->cbRegex->setEnabled(true);
        ui->cbCase->setChecked(false);
        ui->cbCase->setEnabled(true);
        ui->cbInvertMatch->setChecked(false);
        ui->cbInvertMatch->setEnabled(true);
        ui->cbBegin->setChecked(false);
        ui->cbBegin->setEnabled(true);
    }
}


void AdvancedSearchDialog::on_cbBegin_clicked()
{
    if(ui->cbBegin->isChecked())
    {
        ui->cbBackwards->setChecked(false);
    }

}


void AdvancedSearchDialog::on_cbRegex_clicked()
{
    if(ui->cbRegex->isChecked())
    {
        ui->cbBackwards->setChecked(false);
    }
    else
    {
        ui->cbCase->setChecked(false);
    }
}

