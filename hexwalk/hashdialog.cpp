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
#include "hashdialog.h"
#include "qevent.h"
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
void HashDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        haltCalc = true;
        this->hide();
    } else {
        QDialog::keyPressEvent(event);
    }
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

