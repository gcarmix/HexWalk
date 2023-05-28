#include "hashdialog.h"
#include "ui_hashdialog.h"
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>
HashDialog::HashDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HashDialog)
{
    ui->setupUi(this);
}

HashDialog::~HashDialog()
{
    delete ui;
}

void HashDialog::calculate(QString filepath)
{
    QFile in(filepath);
    haltCalc = false;
    ui->md5Lbl->setText("");
    ui->sha1Lbl->setText("");
    ui->sha256Lbl->setText("");
    if (in.open(QIODevice::ReadOnly)) {
        qint64 fileSize = in.size();
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
        ui->progressBar->show();
        QCryptographicHash hashmd5(QCryptographicHash::Md5);
        QCryptographicHash hashsha1(QCryptographicHash::Sha1);
        QCryptographicHash hashsha256(QCryptographicHash::Sha256);
        hashmd5.reset();
        hashsha1.reset();
        hashsha256.reset();
        char buf[4096];
        qint64 bytesRead;
        qint64 totalBytes = 0;
        while ((bytesRead = in.read(buf, 4096)) > 0) {
            if(haltCalc == true)
            {
                break;
            }
            totalBytes+=bytesRead;
            ui->progressBar->setValue(100.0*totalBytes/fileSize);
            hashmd5.addData(buf, bytesRead);
            hashsha1.addData(buf, bytesRead);
            hashsha256.addData(buf, bytesRead);
            QCoreApplication::processEvents();

        }
        ui->progressBar->hide();
        if(haltCalc == false)
        {
        ui->md5Lbl->setText(hashmd5.result().toHex());
        ui->sha1Lbl->setText(hashsha1.result().toHex());
        ui->sha256Lbl->setText(hashsha256.result().toHex());
        }
        else
        {
        haltCalc = false;
        }

        in.close();
    }
}

void HashDialog::on_pushButton_clicked()
{
    haltCalc = true;
}


void HashDialog::on_pushButton_2_clicked()
{
    haltCalc = true;
    this->hide();
}

