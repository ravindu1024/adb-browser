#include "pcnavigator.h"

PcNavigator::PcNavigator(WINDOW *statuswindow, int windowH, int windowW)
    : DirectoryNavigator(statuswindow, windowH, windowW)
{

}

PcNavigator::~PcNavigator()
{

}

void PcNavigator::TokenizeLine(string& input, FileData *fileData)
{
    vector<string> tokens = splitLine(input);

    fileData->permissions = tokens[0];
    fileData->usergroup = tokens[2];
    fileData->user = tokens[3];

    if(input.at(0) == '-')
    {
        fileData->size = tokens[4];
    }
    else
    {
        fileData->size = "0";
    }

    double size = 0;
    std::stringstream(fileData->size) >> size;
    if(size > 1000000000)
    {
        size = size / (1024*1024*1024.0);
        fileData->size = NumberToString(size) + string(" G");
    }
    else if(size > 1000000)
    {
        size = size / (1024*1024.0);
        fileData->size = NumberToString(size) + string(" M");
    }
    else if (size > 1000)
    {
        size = size / 1024.0;
        fileData->size = NumberToString(size) + string(" K");
    }
    else
    {
        fileData->size = NumberToString(size) + string(" B");
    }


    size_t sss = input.find_first_of('"');
    string filename = input.substr(sss+1);
    filename.erase(std::remove(filename.begin(), filename.end(), '"'), filename.end());

    fileData->filename = filename;
    fileData->formattedFilename = ReplaceSpaces(filename);

    if(input.at(0) == 'l')
    {
        size_t eee = input.find_first_of('>');
        fileData->formattedFilename = ReplaceSpaces(input.substr(sss+1, (eee-sss-4)));
    }

}


void PcNavigator::OpenFile(string pwd, string filename)
{
    filename = pwd+filename;
    string command = "gedit " + filename;
    system(command.c_str());
}

void PcNavigator::PasteFiles(string pwd, vector<string>& files)
{

    for(u_int32_t i=0; i<files.size(); i++)
    {
        string command = string(ADB) + " pull "+files[i] + " " + pwd;
        system(command.c_str());
    }
}

void PcNavigator::DeleteFile(string filename)
{
    string command = "rm " + filename;
    system(command.c_str());
}

string PcNavigator::GetDeviceName()
{
    char buff[1024] = "";
    gethostname(buff, sizeof(buff));
    return string(buff);
}

