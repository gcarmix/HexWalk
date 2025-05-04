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
#ifndef CONVERTERWIDGET_H
#define CONVERTERWIDGET_H

#include <QDockWidget>

namespace Ui {
class ConverterWidget;
}

class ConverterWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ConverterWidget(QWidget *parent = nullptr);
    void update(QString hexString);
    ~ConverterWidget();

private slots:

    void on_decTextEdit_textChanged(const QString &arg1);
    void on_hexTextEdit_textChanged(const QString &arg1);
    void on_binTextEdit_textChanged(const QString &arg1);
    void on_checkBox_be_stateChanged(int arg1);
    //void update(QByteArray ba);

private:
    Ui::ConverterWidget *ui;
    bool isBE=false;
    bool isSigned=false;
    void updateDec();
    void updateHex();
    QString binValue;
    QString decValue;
    QString hexValue;
};

#endif // CONVERTERWIDGET_H
