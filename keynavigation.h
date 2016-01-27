#pragma once

#include <stack>
#include <string>
#include "pcnavigator.h"
#include "adbnavigator.h"
#include <ncurses.h>



using namespace std;


struct History
{
    string path;
    int selection;
    int minimum;
    int maximum;

    History(string p, int sel, int min, int max)
    {
        path = p;
        selection = sel;
        minimum = min;
        maximum = max;
    }
};


class KeyHandler
{
public:

    KeyHandler(WINDOW* statuswindow, WINDOW *mainwindow, int m_windowH, int m_windowW, DeviceType device);
    ~KeyHandler();

    void Forward();
    void Backward();
    void Up();
    void Down();
    void RefreshFiles();
    void RefreshView();
    void Disable();
    void Enable();
    void ViewFile();
    string SelectFile();
    void Delete();
    void PasteFiles(vector<string> &files);
    string GetDeviceName();



private:
    int m_highlighted;
    stack<History> m_stCurrentFolder;
    DirectoryNavigator* m_DirectoryNavigator;
    int m_windowH;
    int m_windowW;
    int m_currentSelection;
    string m_strPwd;
    WINDOW* m_mainWindow;
    WINDOW* m_statusWindow;

};
