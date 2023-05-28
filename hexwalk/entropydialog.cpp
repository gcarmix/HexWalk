#include "entropydialog.h"
#include "ui_entropydialog.h"
#include <QLineSeries>
#include <QChart>

EntropyDialog::EntropyDialog(QHexEdit * hexedit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntropyDialog)
{
    _hexed = hexedit;
    ui->setupUi(this);

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
void EntropyDialog::calculate()
{
    qint64 cursor = 0;
    qint64 dataSize = 1024;
    QLineSeries *series = new QLineSeries();
    progrDialog = new QProgressDialog("Entropy calculation in progress...","Cancel",0,100,this);
    progrDialog->setValue(0);
    progrDialog->show();

    while(cursor < _hexed->getSize())
    {
       dataSize = 1024;
       if(dataSize > (_hexed->getSize()-cursor))
       {
           dataSize = (_hexed->getSize()-cursor);
       }
       QByteArray data = _hexed->dataAt(cursor,dataSize);
       series->append(double(cursor),blockEntropy(&data));
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
    chart->axes(Qt::Vertical).back()->setRange(0.0,1.1);
    chart->axes(Qt::Vertical).back()->setLabelsBrush(QBrush(QColor("lightgray")));
    chart->axes(Qt::Horizontal).back()->setLabelsBrush(QBrush(QColor("lightgray")));
    chart->setTitle("Entropy chart");

    chart->setBackgroundBrush(QBrush(QColor("black")));
    chart->setTitleBrush(QBrush(QColor("lightgray")));
    entropyView = new EntropyChart(chart,this);
    entropyView->setRenderHint(QPainter::Antialiasing);

    QLayoutItem *item;

    while((item=ui->verticalLayout->takeAt(0)) != NULL)
    {
       delete item;
    }
    ui->verticalLayout->addWidget(entropyView);
    connect(entropyView,SIGNAL(mousePressed(qint64)),this,SLOT(mousePressed(qint64)));

}

void EntropyDialog::mousePressed(qint64 value)
{
    _hexed->setCursorPosition(2*value);
    _hexed->ensureVisible();
}
EntropyDialog::~EntropyDialog()
{

    delete ui;
}



void EntropyDialog::on_buttonBox_clicked(QAbstractButton *button)
{

}

