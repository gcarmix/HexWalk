#include "bytepattern.h"

#include <QDebug>
#include <regex>

BytePattern::BytePattern(QHexEdit * qhexptr)
{
    hexEdit = qhexptr;
}


using namespace std;

std::vector<std::string> BytePattern::tokenizeString(const std::string& input, const std::string& delimiters) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, ' ')) {
        std::string sanitizedToken;

        // Remove spaces from the token
        for (char c : token) {
            if (c != ' ') {
                sanitizedToken += c;
            }
        }

        // Split the sanitized token based on delimiters
        size_t start = 0;
        size_t end = sanitizedToken.find_first_of(delimiters);

        while (end != std::string::npos) {
            tokens.push_back(sanitizedToken.substr(start, end - start));
            start = end + 1;
            end = sanitizedToken.find_first_of(delimiters, start);
        }

        if (start < sanitizedToken.length()) {
            tokens.push_back(sanitizedToken.substr(start));
        }
    }

    return tokens;
}
void BytePattern::reset(){
    colorTag.clear();
}
bool BytePattern::isHexString(std::string x)
{
    std::regex hexRegex("^0x[0-9A-Fa-f]+$");

    if (!std::regex_match(x, hexRegex)) {
        return false;
    }
    return true;
}
std::string BytePattern::hexToDecimal(const std::string& hexString) {
    std::string hexValue = hexString.substr(2);

    std::stringstream ss;
    ss << std::hex << hexValue;

    int decimalValue;
    ss >> decimalValue;

    std::stringstream resultStream;
    resultStream << decimalValue;
    std::string decimalString = resultStream.str();

    return decimalString;
}

/*Search for possible references to resolve*/
int BytePattern::resolveReferences(){
    bool ok;
    int pos;
    bool found = false;
    for(int i=0;i<colorTag.size();i++)
    {
        ColorTag tag = colorTag.at(i);
        if(!isDigit(tag.obj.offset))
        {
            //Offset is not a number, check if there are variables to substitute
            vector<string> foundTokens = tokenizeString(tag.obj.offset,"+-*/()");
            for(int k=0;k<(int)foundTokens.size();k++)
            {
                string token = foundTokens.at(k);
                if(!isDigit(token)){
                    //token is not a number, might be a variable
                    if(isHexString(token))
                    {
                        pos = tag.obj.offset.find(token);
                        string decString = hexToDecimal(token);
                        tag.obj.offset.replace(pos,token.length(),decString);
                    }
                    else
                    {
                        found = false;
                        for(int j=0;j<colorTag.size();j++)
                        {
                            //search for a tag with the same name
                            ColorTag temptag = colorTag.at(j);
                            if(temptag.name == token)
                            {
                                if(temptag.type == BE_t || temptag.type == LE_t)
                                {
                                    pos = tag.obj.offset.find(temptag.name);
                                    if(pos != (int)std::string::npos)
                                    {
                                        found = true;
                                        if(temptag.type == LE_t)
                                        {
                                            reverse(temptag.data.begin(),temptag.data.end());
                                        }
                                        tag.obj.offset.replace(pos,temptag.name.length(),to_string(temptag.data.toHex().toLongLong(&ok,16)));
                                        break;
                                    }
                                }
                            }
                        }
                        if(found == false)
                            return i;
                    }
                }
            }

        }
        if(!isDigit(tag.obj.size))
        {
            //Offset is not a number, check if there are variables to substitute
            vector<string> foundTokens = tokenizeString(tag.obj.size,"+-*/()");
            for(int k=0 ; k<(int)foundTokens.size();k++)
            {
                string token = foundTokens.at(k);
                if(!isDigit(token)){
                    //token is not a number, might be a variable
                    if(isHexString(token))
                    {
                        pos = tag.obj.size.find(token);
                        string decString = hexToDecimal(token);
                        tag.obj.size.replace(pos,token.length(),decString);
                    }
                    else
                    {
                        found = false;
                        for(int j=0;j<colorTag.size();j++)
                        {
                            //search for a tag with the same name
                            ColorTag temptag = colorTag.at(j);
                            if(temptag.name == token)
                            {
                                if(temptag.type == BE_t || temptag.type == LE_t)
                                {
                                    pos = tag.obj.size.find(temptag.name);
                                    if(pos != (int)std::string::npos)
                                    {
                                        if(temptag.type == LE_t)
                                        {
                                            reverse(temptag.data.begin(),temptag.data.end());
                                        }
                                        tag.obj.size.replace(pos,temptag.name.length(),to_string(temptag.data.toHex().toLongLong(&ok,16)));
                                        found = true;
                                        break;
                                    }
                                }
                            }
                        }

                        if(found == false)
                            return i;
                    }
                }
            }

        }
        int outsize = evaluateExpression(tag.obj.size);
        int outoffset = evaluateExpression(tag.obj.offset);
        colorTag[i].size = outsize;
        colorTag[i].pos = outoffset;
        int reqSize;
        if(colorTag[i].type == BE_t ||colorTag[i].type == LE_t)
        {
            if(outsize > 8)
            {
                reqSize = 8;
            }
            else
            {
                reqSize = outsize;
            }
        }
        else
        {
            if(outsize > 16)
            {
                reqSize = 16;
            }
            else
            {
                reqSize = outsize;
            }
        }
        colorTag[i].data = hexEdit->dataAt(colorTag[i].pos,reqSize);
        //qInfo()<<"NAME:"<<QString::fromStdString(tag.obj.name)<<"\n";
        //qInfo()<<"SIZE:"<<outsize<<"\n";
        //qInfo()<<"OFFSET:"<<outoffset<<"\n";
    }
    return 0;
}

bool BytePattern::isValidName(std::string name)
{
    if(!isDigit(name))
    {
        if(name.length() > 0)
        {
            if(name[0] >='0' && name[0] <= '9')
            {
                //name can't start with a number
                return false;
            }
            if(strspn( name.c_str(), "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" ) == name.size())
            {
                return true;
            }
        }
    }
    return false;

}

bool BytePattern::isDuplicatedName(std::string name,int excluded_idx)
{
    for(int i=0;i<colorTag.size();i++)
    {
        if(excluded_idx == i)
        {
            continue;
        }
        if(colorTag.at(i).name == name)
        {
            return true;
        }
    }
    return false;

}
bool BytePattern::isValidColor(std::string name)
{
    if(!isDigit(name))
    {
        regex expression("^#[0-9a-fA-F]{6}");
        if(regex_match(name,expression))
            return true;
    }
    return false;

}
bool BytePattern::isDigit(string str){
    if(strspn( str.c_str(), "0123456789" ) == str.size())
    {
        return true;
    }
    return false;
}
int BytePattern::delElement(int idx){
    colorTag.removeAt(idx);
    return 0;
}
int BytePattern::updateElement(int idx,YMLObj obj)
{
    colorTag[idx].obj = obj;
    colorTag[idx].name = obj.name;
    colorTag[idx].color = obj.color;

    if(obj.type == tagStrings[LE_t])
    {
        colorTag[idx].type = LE_t;
    }
    else if(obj.type == tagStrings[BE_t])
    {
        colorTag[idx].type = BE_t;
    }
    else if(obj.type == tagStrings[STRING_t])
    {
        colorTag[idx].type = STRING_t;
    }
    else if(obj.type == tagStrings[HEX_t])
    {
        colorTag[idx].type = HEX_t;
    }
    else
    {
        return -1;
    }

    resolveReferences();
    return 0;

}
int BytePattern::addElement(YMLObj obj){
    ColorTag tag;
    if(!isValidName(obj.name))
    {
        return E_WRONGNAME;
    }
    else if(!isValidColor(obj.color))
    {
        return E_WRONGCOLOR;
    }
    else if(isDuplicatedName(obj.name,-1))
    {
        return E_DUPNAME;
    }
    tag.type = UNDEFINED_t;
    tag.obj = obj;
    tag.name = obj.name;
    tag.size = 0;
    tag.pos = 0;
    tag.color = obj.color;


    if(obj.type == tagStrings[LE_t])
    {
        tag.type = LE_t;
    }
    else if(obj.type == tagStrings[BE_t])
    {
        tag.type = BE_t;
    }
    else if(obj.type == tagStrings[STRING_t])
    {
        tag.type = STRING_t;
    }
    else if(obj.type == tagStrings[HEX_t])
    {
        tag.type = HEX_t;
    }
    else
    {
        return E_WRONGTYPE;
    }

    colorTag.append(tag);
    if(resolveReferences())
    {
        return E_WRONGREF;
    }
    return 0;
}
// Function to check if a character is an operator
bool BytePattern::isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// Function to perform arithmetic operations
int BytePattern::performOperation(char operation, int operand1, int operand2) {
    switch (operation) {
    case '+':
        return operand1 + operand2;
    case '-':
        return operand1 - operand2;
    case '*':
        return operand1 * operand2;
    case '/':
        return operand1 / operand2;
    default:
        return 0;
    }
}

// Function to evaluate the mathematical expression
int BytePattern::evaluateExpression(const string& expression) {
    stack<int> operandStack;
    stack<char> operatorStack;

    for (size_t i = 0; i < expression.length(); ++i) {
        // Ignore whitespaces
        if (expression[i] == ' ')
            continue;

        // If the character is a digit, parse the entire number
        if (isdigit(expression[i])) {
            int operand = 0;
            while (isdigit(expression[i])) {
                operand = operand * 10 + (expression[i] - '0');
                ++i;
            }
            i--;
            operandStack.push(operand);
        }

        // If the character is an operator
        else if (isOperator(expression[i])) {
            while (!operatorStack.empty() && operatorStack.top() != '(' &&
                   ((expression[i] != '*' && expression[i] != '/') || (operatorStack.top() != '+' && operatorStack.top() != '-'))) {
                int operand2 = operandStack.top();
                operandStack.pop();

                int operand1 = operandStack.top();
                operandStack.pop();

                char op = operatorStack.top();
                operatorStack.pop();

                int result = performOperation(op, operand1, operand2);
                operandStack.push(result);
            }
            operatorStack.push(expression[i]);
        }

        // If the character is an opening parenthesis
        else if (expression[i] == '(') {
            operatorStack.push(expression[i]);
        }

        // If the character is a closing parenthesis
        else if (expression[i] == ')') {
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                int operand2 = operandStack.top();
                operandStack.pop();

                int operand1 = operandStack.top();
                operandStack.pop();

                char op = operatorStack.top();
                operatorStack.pop();

                int result = performOperation(op, operand1, operand2);
                operandStack.push(result);
            }
            operatorStack.pop(); // Pop the opening parenthesis
        }
    }

    while (!operatorStack.empty()) {
        int operand2 = operandStack.top();
        operandStack.pop();

        int operand1 = operandStack.top();
        operandStack.pop();

        char op = operatorStack.top();
        operatorStack.pop();

        int result = performOperation(op, operand1, operand2);
        operandStack.push(result);
    }

    return operandStack.top();
}
