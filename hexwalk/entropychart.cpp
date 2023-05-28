#include "entropychart.h"

EntropyChart::EntropyChart(QChart * chart,QWidget *parent):
    QChartView(chart,parent)
{

}

void EntropyChart::mousePressEvent(QMouseEvent * event){
    auto curPoint = QCursor::pos();
    curPoint = this->mapFromGlobal(curPoint);
    auto pickVal = this->mapToScene(curPoint);
    pickVal = this->chart()->mapFromScene(curPoint);
    pickVal = this->chart()->mapToValue(curPoint,this->chart()->series().at(0));

    emit mousePressed(qint64(pickVal.x()));
    QChartView::mousePressEvent(event);
}
