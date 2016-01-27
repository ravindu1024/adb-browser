#include "adbnavigator.h"

AdbNavigator::AdbNavigator(WINDOW *statuswindow, int windowH, int windowW)
    : DirectoryNavigator(statuswindow, windowH, windowW)
{

}

AdbNavigator::~AdbNavigator()
{

}

void AdbNavigator::TokenizeLine(string &input, FileData* fileData)
{

    vector<string> tokens = splitLine(input);

    fileData->permissions = tokens[0];
    fileData->usergroup = tokens[1];
    fileData->user = tokens[2];

    if(input.at(0) == '-')
    {
        fileData->size = tokens[3];
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
        size = size / (1024*1024);
        fileData->size = NumberToString(size) + string(" M");
    }
    else if (size > 1000)
    {
        size = size / 1024;
        fileData->size = NumberToString(size) + string(" K");
    }
    else
    {
        fileData->size = NumberToString(size) + string(" B");
    }


    size_t sss = input.find_first_of(':');
    fileData->filename = input.substr(sss+4);
    fileData->formattedFilename = ReplaceSpaces(input.substr(sss+4));

    if(input.at(0) == 'l')
    {
        size_t eee = input.find_first_of('>');
        fileData->formattedFilename = ReplaceSpaces(input.substr(sss+4, (eee-sss-6)));
    }
}


void AdbNavigator::OpenFile(string pwd, string filename)
{
    string command = string(ADB) + " pull " + pwd+filename;
    system(command.c_str());

    command = "gedit ./" + filename;
    system(command.c_str());

    command = string(ADB) + " push ./" + filename + " "+ pwd+filename;
    system(command.c_str());

    command = "rm ./" + filename;
    system(command.c_str());

}


void AdbNavigator::PasteFiles(string pwd, vector<string> &files)
{
    for(u_int32_t i=0; i<files.size(); i++)
    {
        string command = string(ADB) + " push "+files[i] + " "+pwd;
        system(command.c_str());
    }
}


void AdbNavigator::DeleteFile(string filename)
{
    string command = string(ADB) + " shell rm " + filename;
    system(command.c_str());
}

string AdbNavigator::GetDeviceName()
{
    string result = "";
    char buff[1024] = "";


    //get manufacturer name
    string command = string(ADB) + " shell getprop ro.product.manufacturer";
    FILE* fp = popen(command.c_str(), "r");
    if(fp != NULL)
    {
        fgets(buff, 1024, fp);
        result = string(buff);
    }


    //get model name
    command = string(ADB) + " shell getprop ro.product.model";
    fp = popen(command.c_str(), "r");
    if(fp != NULL)
    {
        fgets(buff, 1024, fp);
        result += " " + string(buff);
    }


    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());

    pclose(fp);
    return result;
}

