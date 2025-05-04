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
#include "entropychart.h"
#include <QDebug>
EntropyChart::EntropyChart(QWidget *parent):
    QChartView(parent)
{

}

void EntropyChart::mousePressEvent(QMouseEvent * event){
    auto curPoint = QCursor::pos();
    curPoint = this->mapFromGlobal(curPoint);
    auto pickVal = this->mapToScene(curPoint);
    pickVal = this->chart()->mapFromScene(curPoint);
    pickVal = this->chart()->mapToValue(curPoint,this->chart()->series().at(0));
emit rubberBandEvent();
    emit mousePressed(qint64(pickVal.x()));

    QChartView::mousePressEvent(event);

}
void EntropyChart::resizeEvent(QResizeEvent *event){

    emit rubberBandEvent();
    QChartView::resizeEvent(event);
}
void EntropyChart::mouseReleaseEvent(QMouseEvent *event){
    emit rubberBandEvent();
    QChartView::mouseReleaseEvent(event);
}

void EntropyChart::mouseMoveEvent(QMouseEvent * event){
    auto curPoint = QCursor::pos();
    curPoint = this->mapFromGlobal(curPoint);
    auto pickVal = this->mapToScene(curPoint);
    pickVal = this->chart()->mapFromScene(curPoint);
    pickVal = this->chart()->mapToValue(curPoint,this->chart()->series().at(0));

    emit mouseMoved(qint64(pickVal.x()));
    QChartView::mouseMoveEvent(event);
}
