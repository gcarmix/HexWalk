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
    explicit EntropyChart(QChart *chart,QWidget *parent = nullptr);

signals:
    void mousePressed(qint64 value);
protected:
    void mousePressEvent(QMouseEvent *event);



};

#endif // ENTROPYCHART_H
