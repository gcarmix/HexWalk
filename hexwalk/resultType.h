#ifndef RESULTTYPE_H
#define RESULTTYPE_H
#include <QString>
#include <qglobal.h>

typedef struct Result_S{
    qint64 cursor;
    QString hexdatastr;
    QString datastr;

}Result_S;

typedef struct BinwalkResult_S{
    qint64 cursor;
    QString datastr;

}BinwalkResult_S;

#endif // RESULTTYPE_H
