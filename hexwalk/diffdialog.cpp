#include "diffdialog.h"
#include "ui_diffdialog.h"
#include <QScrollBar>
#include <QProgressDialog>

DiffDialog::DiffDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiffDialog)
{
    ui->setupUi(this);
}

DiffDialog::~DiffDialog()
{
    delete ui;
}

void DiffDialog::trackCursor1(qint64 pos)
{
    int scrollpos = ui->hexEdit1->verticalScrollBar()->value();
    ui->hexEdit2->verticalScrollBar()->setValue(scrollpos);
}
void DiffDialog::trackCursor2(qint64 pos)
{
    int scrollpos = ui->hexEdit2->verticalScrollBar()->value();
    ui->hexEdit1->verticalScrollBar()->setValue(scrollpos);
}
void DiffDialog::setFiles(QString file1str, QString file2str)
{

    ui->hexEdit1->setAddressWidth(8);
    ui->hexEdit1->setBytesPerLine(8);
    ui->hexEdit1->setHexCaps(true);
    ui->hexEdit1->setAddressAreaColor(QColor("#545454"));
    ui->hexEdit1->setAddressFontColor(QColor("#f0f0f0"));
    ui->hexEdit1->setAsciiFontColor(QColor("#00ff5e"));
    ui->hexEdit1->setHexFontColor(QColor("#00ff5e"));
    ui->hexEdit1->setHighlightingColor(QColor("#540c00"));
    ui->hexEdit1->setFont(QFont("Courier",12));
    ui->hexEdit1->setHighlighting(true);
    ui->hexEdit1->setOverwriteMode(true);
    ui->hexEdit2->setAddressWidth(8);
    ui->hexEdit2->setBytesPerLine(8);
    ui->hexEdit2->setHexCaps(true);
    ui->hexEdit2->setAddressAreaColor(QColor("#545454"));
    ui->hexEdit2->setAddressFontColor(QColor("#f0f0f0"));
    ui->hexEdit2->setAsciiFontColor(QColor("#00ff5e"));
    ui->hexEdit2->setHexFontColor(QColor("#00ff5e"));
    ui->hexEdit2->setHighlightingColor(QColor("#540c00"));
    ui->hexEdit2->setFont(QFont("Courier",12));
    ui->hexEdit2->setHighlighting(true);
    ui->hexEdit2->setOverwriteMode(true);
    ui->hexEdit1->setReadOnly(true);
    ui->hexEdit2->setReadOnly(true);


    file1.setFileName(file1str);

    file2.setFileName(file2str);
    qInfo()<<file1.fileName();
    qInfo()<<file2.fileName();
    ui->hexEdit1->setData(file1);
    ui->hexEdit2->setData(file2);
    connect(ui->hexEdit1,SIGNAL(currentAddressChanged(qint64)),this,SLOT(trackCursor1(qint64)));
    connect(ui->hexEdit2,SIGNAL(currentAddressChanged(qint64)),this,SLOT(trackCursor2(qint64)));

}

void DiffDialog::on_nextDiffBtn_clicked()
{
    progrDialog = new QProgressDialog("Task in progress...","Cancel",0,100,this);
    progrDialog->setValue(0);
    progrDialog->setModal(true);
    progrDialog->show();
    qint64 pos = ui->hexEdit1->cursorPosition();
    progrDialog->setValue(100.0*(double)(pos/2)/ui->hexEdit1->getSize());
    int idx = 0;
    while(pos/2 < ui->hexEdit1->getSize())
    {
        QByteArray data1 = ui->hexEdit1->dataAt(pos/2,1024);
        QByteArray data2 = ui->hexEdit2->dataAt(pos/2,1024);
        if(!data1.compare(data2))
        {
            pos+=2*1024;
        }
        else
        {
            if(ui->hexEdit2->dataAt(pos/2,1)!=ui->hexEdit1->dataAt(pos/2,1))
            {
                ui->hexEdit2->setCursorPosition(pos);
                ui->hexEdit1->ensureVisible();
                ui->hexEdit2->ensureVisible();
                if(idx != 0)
                    break;
            }
            pos+=2;
        }
        ui->hexEdit1->setCursorPosition(pos);
        idx++;


        if(progrDialog->wasCanceled())
        {
            break;
        }
        progrDialog->setValue((int)(100.0*(double)(pos/2)/ui->hexEdit1->getSize()));
        QCoreApplication::processEvents();

    }
    progrDialog->hide();

}


void DiffDialog::on_exitBtn_clicked()
{
    this->hide();
}


void DiffDialog::on_prevDiffBtn_clicked()
{
    qint64 pos = ui->hexEdit1->cursorPosition();
    int idx = 0;
    while(pos > 0)
    {
        if(ui->hexEdit2->dataAt(pos/2,1)!=ui->hexEdit1->dataAt(pos/2,1))
        {
            ui->hexEdit2->setCursorPosition(pos);
            ui->hexEdit1->ensureVisible();
            ui->hexEdit2->ensureVisible();
            if(idx != 0)
                break;
        }
        pos-=2;
        ui->hexEdit1->setCursorPosition(pos);
        idx++;
    }
}

