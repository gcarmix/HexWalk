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
#ifndef ENTROPYCHART_H
#define ENTROPYCHART_H
#include <QtCharts>
#include <QChartView>
#include <QRubberBand>
QT_CHARTS_USE_NAMESPACE
class EntropyChart : public QChartView
{
Q_OBJECT
public:
    explicit EntropyChart(QWidget *parent = nullptr);

signals:
    void mousePressed(qint64 value);
    void mouseMoved(qint64 address);
    void rubberBandEvent();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);




};

#endif // ENTROPYCHART_H
