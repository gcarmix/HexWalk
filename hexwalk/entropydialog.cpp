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
#include "entropydialog.h"
#include "ui_entropydialog.h"
#include <QLineSeries>
#include <QChart>
#include <QDebug>

EntropyDialog::EntropyDialog(QHexEdit * hexedit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntropyDialog)
{
    _hexed = hexedit;
    ui->setupUi(this);
    connect(parent,SIGNAL(fileLoaded()),this,SLOT(refresh()));
}
double EntropyDialog::blockEntropy(QByteArray * data)
{
    int seen[256]={0};
    double p_x;
    double entropy = 0;
    for(int i=0;i<data->length();i++){
        seen[uchar(data->at(i))]++;
    }

    for(int i=0;i<256;i++){
        p_x = double(seen[i])/data->length();
        if(seen[i] > 0)
       {
           entropy -= p_x*log2(p_x);
       }
    }

    return entropy/8;

}
void EntropyDialog::refresh()
{
    if(this->isVisible())
    {
        calculate();
    }
}
void EntropyDialog::calculate()
{
    qint64 cursor = 0;
    qint64 dataSize = 1024;
    qint64 blockSize = 1024;

    if(_hexed->getSize() > 32*1024*1024)
    {
        blockSize = 4096;
    }
    if(_hexed->getSize() > 128*1024*1024)
    {
        blockSize = 16384;
    }
    series = new QLineSeries();
    progrDialog = new QProgressDialog("Entropy calculation in progress...","Cancel",0,100,this);
    progrDialog->setValue(0);
    progrDialog->show();

    while(cursor < _hexed->getSize())
    {
       dataSize = blockSize;
       if(dataSize > (_hexed->getSize()-cursor))
       {
           dataSize = (_hexed->getSize()-cursor);
       }
       QByteArray data = _hexed->dataAt(cursor,dataSize);
       series->append(cursor,blockEntropy(&data));
       cursor+=dataSize;
       progrDialog->setValue(int(100.0*(double(cursor)/double(_hexed->getSize()))));
       QCoreApplication::processEvents();
       if(progrDialog->wasCanceled())
           break;
    }

    progrDialog->cancel();
    series->setColor(Qt::green);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).back()->setRange(0.0,1.0);
    chart->axes(Qt::Vertical).back()->setLabelsBrush(QBrush(QColor("lightgray")));
    chart->axes(Qt::Horizontal).back()->setLabelsBrush(QBrush(QColor("lightgray")));
    chart->setTitle("Entropy chart");

    chart->setBackgroundBrush(QBrush(QColor("black")));
    chart->setTitleBrush(QBrush(QColor("lightgray")));
    //entropyView = new EntropyChart(chart,this);
    //entropyView->setRenderHint(QPainter::Antialiasing);
    ui->entropyChart->setChart(chart);
    ui->entropyChart->setRubberBand(QChartView::HorizontalRubberBand);
    ui->entropyChart->setDragMode(QGraphicsView::ScrollHandDrag);
    //QLayoutItem *item;

    /*while((item=ui->verticalLayout->takeAt(0)) != NULL)
    {
       delete item;
    }
    ui->verticalLayout->addWidget(entropyView);*/
    connect(ui->entropyChart,SIGNAL(mousePressed(qint64)),this,SLOT(mousePressed(qint64)));
    connect(ui->entropyChart,SIGNAL(mouseMoved(qint64)),this,SLOT(mouseMoved(qint64)));
    connect(ui->entropyChart,SIGNAL(rubberBandEvent()),this,SLOT(limitZoomOut()));

}

void EntropyDialog::mousePressed(qint64 value)
{
    _hexed->setCursorPosition(2*value);
    _hexed->ensureVisible();
}
void EntropyDialog::mouseMoved(qint64 address)
{
    if(address > 0 && address < _hexed->getSize()){

        ui->AddressEdt->setText(QString::asprintf("%lld",address));
       ui->ValueEdt->setText(QString::asprintf("%3.2f",findClosestPoint(series,address).y()));
    }
}

void EntropyDialog::limitZoomOut() {

    // Limit zoom out to 1.0x



       QValueAxis *axisX = dynamic_cast<QValueAxis *>(ui->entropyChart->chart()->axisX());
       if (axisX) {

           if(axisX->min()< 0.0)
           {
               ui->entropyChart->chart()->axes(Qt::Horizontal).back()->setRange(0.0,axisX->max());
           }
           if(axisX->max()>_hexed->getSize())
           {
               ui->entropyChart->chart()->axes(Qt::Horizontal).back()->setRange(axisX->min(),_hexed->getSize());
           }
       }

}

QPointF EntropyDialog::findClosestPoint(QLineSeries* lineSeries, qreal x) {
    QPointF closestPoint;
    qreal minDistance = std::numeric_limits<qreal>::max();

    // Itera sui punti della serie e trova il punto più vicino
    for (const QPointF& point : lineSeries->points()) {
       qreal distance = qAbs(point.x() - x);
       if (distance < minDistance) {
           minDistance = distance;
           closestPoint = point;
       }
    }

    return closestPoint;
}

EntropyDialog::~EntropyDialog()
{

    delete ui;
}



void EntropyDialog::on_buttonBox_clicked(QAbstractButton *button)
{

}


void EntropyDialog::on_entropyChart_mousePressed(qint64 )
{

}

