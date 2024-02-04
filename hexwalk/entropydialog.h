#ifndef ENTROPYDIALOG_H
#define ENTROPYDIALOG_H

#include <QDialog>
#include <QtCharts>
#include <QProgressDialog>
#include "../src/qhexedit.h"
#include "entropychart.h"

namespace Ui {
class EntropyDialog;
}

class EntropyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EntropyDialog(QHexEdit * hexedit,QWidget *parent = nullptr);
    ~EntropyDialog();
    void calculate();
public slots:
    void mousePressed(qint64 value);
    void mouseMoved(qint64 address);
    void limitZoomOut();
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_entropyChart_mousePressed(qint64 );

private:
    Ui::EntropyDialog *ui;
    QHexEdit * _hexed;
    double blockEntropy(QByteArray * data);
    EntropyChart *entropyView;
    QProgressDialog *progrDialog;
    QLineSeries *series = NULL;
    QPointF findClosestPoint(QLineSeries* lineSeries, qreal x);
};

#endif // ENTROPYDIALOG_H
