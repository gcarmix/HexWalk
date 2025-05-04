#ifndef COLORTAG_H
#define COLORTAG_H

#include <string>
#include "tagparser.hpp"
#include <QByteArray>


typedef enum TagType_e
{
    UNDEFINED_t,
    STRING_t,
    BE_t,
    LE_t,
    HEX_t
}TagType_e;

const string tagStrings[]={"undefined","string","beint","leint","hex"};

class ColorTag{
public:

    string name;
    int64_t pos;
    int64_t size;
    string color;
    TagType_e type;
    QByteArray data;
    YMLObj obj;
};






#endif // COLORTAG_H
