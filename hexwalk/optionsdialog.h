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
#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtCore>
#include <QDialog>
#include <QAbstractButton>
namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QSettings * appSettings,QWidget *parent = 0);
    ~OptionsDialog();
    Ui::OptionsDialog *ui;
    void show();

public slots:
    virtual void accept();

private slots:
    void on_pbHighlightingColor_clicked();
    void on_pbAddressAreaColor_clicked();
    void on_pbAddressFontColor_clicked();
    void on_pbAsciiAreaColor_clicked();
    void on_pbAsciiFontColor_clicked();
    void on_pbHexFontColor_clicked();
    void on_pbSelectionColor_clicked();
    void on_pbWidgetFont_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    void readSettings();
    void writeSettings();
    void setColor(QWidget *widget, QColor color);
    QSettings * appSettings;
};

#endif // OPTIONSDIALOG_H
