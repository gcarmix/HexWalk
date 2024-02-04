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
