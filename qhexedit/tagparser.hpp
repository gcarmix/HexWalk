#ifndef TAGPARSER_HPP
#define TAGPARSER_HPP
#include <iostream>
#include <vector>
#include <string>
using namespace std;

enum Parser_States
{
    START,
    INSIDE_OBJ
};
class YMLObj
{
    public:
    string name;
    string size;
    string offset;
    string color;
    string type;

};
class YMLParser
{
    public:
    YMLParser();
    int loadFile(string filename);
    void saveFile(std::string filename);
    vector<YMLObj> ymlobj;
    private:
    bool isComment(std::string line);
    std::string removeSpaces(std::string input);

};
#endif
