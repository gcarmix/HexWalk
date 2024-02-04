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
