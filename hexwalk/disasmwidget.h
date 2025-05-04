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
#ifndef DISASMWIDGET_H
#define DISASMWIDGET_H

#include <QDockWidget>
#include "../qhexedit/qhexedit.h"
namespace Ui {
class DisasmWidget;
}

class DisasmWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DisasmWidget(QHexEdit * hexedit,QWidget *parent = nullptr);
    ~DisasmWidget();
    void capst(void);
    void refresh(void);
private slots:
    void on_btnDisasm_clicked();
private:
    Ui::DisasmWidget *ui;
    QHexEdit * _hexedit;
};

#endif // DISASMWIDGET_H
