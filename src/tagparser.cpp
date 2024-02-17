#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include "tagparser.hpp"
#include <QDebug>

YMLParser::YMLParser()
{

}
void YMLParser::saveFile(std::string filename)
{
    //qInfo()<<"Filename: "<<QString::fromStdString(filename)<<"\n";
    ofstream fw(filename,std::ofstream::out);
    if(fw.is_open())
    {
        for(int i=0; i <  ymlobj.size();i++)
        {
            fw << "- name: " << ymlobj.at(i).name <<"\n";
            fw << "  offset: " << ymlobj.at(i).offset <<"\n";
            fw << "  size: " << ymlobj.at(i).size <<"\n";
            fw << "  color: " << ymlobj.at(i).color <<"\n";
            fw << "  type: " << ymlobj.at(i).type <<"\n";
            //qInfo() << "#"<<"\n";

        }
        fw.close();
    }
}
bool YMLParser::isComment(std::string line){
    for(int k=0;k<line.size();k++)
    {
        if(line[k] == '#')
        {
            return true;
        }
        else if(line[k] == '\n')
        {
            return true;
        }
        else if(line[k] == ' ')
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

std::string YMLParser::removeSpaces(std::string input) {

    input.erase(input.begin(), std::find_if(input.begin(), input.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));

    input.erase(std::find_if(input.rbegin(), input.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }).base(), input.end());

    return input;
}
int YMLParser::loadFile(std::string filename)
{
    ifstream ymlfile(filename);
    string line;
    int linecount = 0;
    Parser_States parse_state = START;
    ymlobj.clear();
    while (getline(ymlfile,line))
    {
                linecount++;
                if(!isComment(line))
                {
                    if(line.size() < 2)
                    {
                        cout<<"\n *Error parsing line "<<linecount<<"\n";
                        return linecount;
                    }
                    if(line[0] == '-')
                    {
                        if(line.find("- name") !=std::string::npos)
                        {
                            if(line.find(":") != string::npos)
                            {
                                std::string name = line.substr(line.find(":") + 1);
                                name = removeSpaces(name);
                            YMLObj obj;
                            obj.name = name;
                            ymlobj.push_back(obj);
                            parse_state = INSIDE_OBJ;
                            continue;
                            }
                            else
                            {
                                cout<<"\n **Error parsing line "<<linecount<<"\n";
                                return linecount;
                            }
                            
                        }
                    }
                    else if(line[0]==' ' && line[1]==' ')
                    {
                        if (parse_state == INSIDE_OBJ)
                        {
                            int pos = line.find(":");
                            if(pos > 2 && pos != string::npos)
                            {
                                std::string key = line.substr(2,pos - 2);
                                std::string value = line.substr(pos+1);
                                value.erase(remove(value.begin(), value.end(), ' '), value.end());
                                cout<<"KEY: "<<key<<" VALUE: "<< value<<"\n";
                                if(key == "offset")
                                {
                                    ymlobj.back().offset = value; 
                                }
                                else if(key == "size")
                                {
                                    ymlobj.back().size = value;
                                }
                                else if(key == "color")
                                {
                                    ymlobj.back().color = value;
                                }
                                else if(key == "type")
                                {
                                    ymlobj.back().type = value;
                                }
                            }
                            else
                            {
                                cout<<"\n *****Error parsing line "<<linecount<<"\n";
                                return linecount;
                                }
                        }
                        else
                        {
                            cout<<"\n ***Error parsing line "<<linecount<<"\n";
                            return linecount;
                        }
                        
                        
                    }
                    else
                    {
                        cout<<"\n ****Error parsing line "<<linecount<<"\n";
                        return linecount;
                    }
                    
                }

    }
    return 0;
}
