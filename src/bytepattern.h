#ifndef BYTEPATTERN_H
#define BYTEPATTERN_H
#include "qhexedit.h"
#include <QList>
#include <stack>
#include <sstream>
#include <QColor>
#include "colortag.h"
#include "tagparser.hpp"
#include <exception>

class BPException: public std::exception{
private:
    std::string message;
public:
    BPException(std::string msg):message(msg) {}
    std::string what(){
        return message;
    }

};

enum BPerrors
{
    E_NONE = 0,
    E_WRONGNAME = -1,
    E_WRONGTYPE = -2,
    E_WRONGREF = -3,
    E_WRONGCOLOR = -4,
    E_EMPTYOFFSET = -5,
    E_EMPTYSIZE = -6,
    E_DUPNAME = -7,

};

class BytePattern{
public:
    BytePattern(QHexEdit * qhexptr);
    bool isOperator(char c);
    int performOperation(char operation, int operand1, int operand2);
    int evaluateExpression(const string& expression);
    int updateTags();
    int addElement(YMLObj obj);
    bool isDigit(string str);
    QList<ColorTag> colorTag;
    int resolveReferences();
    void reset();
    int delElement(int idx);
    int updateElement(int idx,YMLObj obj);
    YMLParser ymlParser;
    bool isValidName(std::string name);
    bool isValidColor(std::string name);
    bool isDuplicatedName(std::string name,int excluded_idx);
private:
    vector<string> tokenizeString(const string& input, const string& delimiters);
    std::string hexToDecimal(const std::string& hexString);
    bool isHexString(std::string x);
    QHexEdit *hexEdit;
};


#endif // BYTEPATTERN_H
