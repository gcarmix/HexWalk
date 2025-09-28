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
#ifndef BYTEMAP_H
#define BYTEMAP_H
#include <QAbstractScrollArea>
#include <QPen>
#include <QBrush>
#include "../qhexedit/qhexedit.h"

class ByteMap: public QAbstractScrollArea
{
    Q_OBJECT
public:
    ByteMap(QWidget *parent=0);
    void loadBytes(QHexEdit * hexedit);
    qint64 cursorPosition(QPoint pos);
    void setCursorPosition(qint64 actpos);
    qint64 getCurrentPosition();
    void setBytesPerLine(int value);
    void setPixelSize(int value);
    bool colored = false;
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void showEvent(QShowEvent *event);
signals:
    void mouseEvent();
    void mousePress();
private:
    int _rowsShown;
    QColor _jetColor(int value);
    int _pxHeight;
    int _pxWidth;
    int _bytesPerLine;
    QHexEdit * _hexedit;
    qint64 _currentPos;
    QPoint mPoint;
private slots:
    void adjust();
};

#endif // BYTEMAP_H
