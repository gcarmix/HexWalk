#ifndef BYTEMAP_H
#define BYTEMAP_H
#include <QAbstractScrollArea>
#include <QPen>
#include <QBrush>
#include "../src/qhexedit.h"

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
