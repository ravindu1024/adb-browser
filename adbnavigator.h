#pragma once
#include "directorynavigator.h"

class AdbNavigator : public DirectoryNavigator
{
public:
    AdbNavigator(WINDOW* statuswindow, int windowH, int windowW);
    ~AdbNavigator();

protected:
    void    TokenizeLine    (string& input, FileData* fileData);
    string  GetCommandString        () { return string(ADB) + string(" shell ls -l "); }
    void    OpenFile                (string pwd, string filename);
    void    PasteFiles              (string pwd, vector<string>& files);
    void    DeleteFile              (string filename);
    string  GetDeviceName();

};

