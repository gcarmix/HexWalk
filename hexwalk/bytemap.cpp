#include "bytemap.h"
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>

ByteMap::ByteMap(QWidget *parent) : QAbstractScrollArea(parent)
{
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(adjust()));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(adjust()));
    _pxHeight = 3;
    _pxWidth = 3;
    _bytesPerLine = 256;
    _currentPos = 0;
    setMouseTracking(true);
}
void ByteMap::loadBytes(QHexEdit * hexedit)
{
    verticalScrollBar()->setValue(0);
    _hexedit = hexedit;
    adjust();
    viewport()->repaint();
}

void ByteMap::adjust()
{
    _rowsShown = ((viewport()->height()-_pxHeight)/_pxHeight);
    int lineCount = (int)(_hexedit->getSize() / (qint64)_bytesPerLine) + 1;
    verticalScrollBar()->setRange(0, lineCount - _rowsShown);
    verticalScrollBar()->setPageStep(_rowsShown);
}

void ByteMap::showEvent(QShowEvent *event)
{
    // Call the base class implementation
    QAbstractScrollArea::showEvent(event);

    // Perform actions that depend on the correct size of the viewport here
    // Example: Print the correct viewport size
    adjust();
}
void ByteMap::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());

    int value = verticalScrollBar()->value();
    qint64 _bPosFirst = (qint64)value * _bytesPerLine;
    for(int i=0;i<_rowsShown;i++)
    {

        QByteArray rowdata  = _hexedit->dataAt(_bPosFirst+i*_bytesPerLine,_bytesPerLine);
        for(int k=0;k<rowdata.size();k++)
        {
            if((_bPosFirst + i*_bytesPerLine + k) > _hexedit->getSize() )
            {
                break;
            }
            unsigned char pix = rowdata.at(k);
            painter.fillRect(QRect(k*_pxHeight, i*_pxHeight, _pxHeight, _pxHeight), QColor(pix,pix,pix));
            if(_hexedit->getSelectionEnd() - _hexedit->getSelectionBegin() > 0)
            {
                qint64 tpos = _bPosFirst+i*_bytesPerLine + k;
                if(tpos >= _hexedit->getSelectionBegin() && tpos < _hexedit->getSelectionEnd())
                {
                    painter.fillRect(QRect(k*_pxHeight, i*_pxHeight, _pxHeight, _pxHeight), QColor(100,100,240,180));
                }

            }
        }
    }

    if(verticalScrollBar()->value()*_bytesPerLine + mPoint.x()/_pxHeight+_bytesPerLine*mPoint.y()/_pxHeight < _hexedit->getSize())
        painter.fillRect(QRect(mPoint.x(), mPoint.y(), _pxHeight, _pxHeight), QColor(255,255,0));
}
qint64 ByteMap::cursorPosition(QPoint pos){
    qint64 actPos = (qint64)(pos.x()/_pxHeight) + (qint64)(pos.y()/_pxHeight)*_bytesPerLine+(qint64)verticalScrollBar()->value()*_bytesPerLine;

    return actPos;
}
void ByteMap::setCursorPosition(qint64 actpos){
    _currentPos = actpos;
}
qint64 ByteMap::getCurrentPosition(){
    return _currentPos;
}

void ByteMap::mouseMoveEvent(QMouseEvent *event){

    viewport()->update();
    mPoint = event->pos();
    qint64 actPos = cursorPosition(event->pos());
    if (actPos >= 0)
    {
        setCursorPosition(actPos);
        emit mouseEvent();
    }
}

void ByteMap::mousePressEvent(QMouseEvent *event){

    viewport()->update();
    qint64 actPos = cursorPosition(event->pos());
    if (actPos >= 0)
    {
        setCursorPosition(actPos);
        emit mousePress();
    }
}

void ByteMap::setBytesPerLine(int value){

    _bytesPerLine = value;
}
