#pragma once

#include "directorynavigator.h"
#include "unistd.h"

class PcNavigator : public DirectoryNavigator
{
public:

    PcNavigator(WINDOW *statuswindow, int windowH, int windowW);
    ~PcNavigator();
    string GetSelectedFileName(int line);

protected:
    void    TokenizeLine    (string& input, FileData* fileData);
    string  GetCommandString        () { return "ls -lQ "; }
    void    OpenFile                (string pwd, string filename);
    void    PasteFiles              (string pwd, vector<string>& files);
    void    DeleteFile              (string filename);
    string  GetDeviceName();
};

