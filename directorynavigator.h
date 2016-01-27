#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include "ncurses.h"
#include "CommonDefines.h"



using namespace std;

struct FileData
{
    string permissions;
    string user;
    string usergroup;
    string size;
    string filename;
    string formattedFilename;
};

class DirectoryNavigator
{
public:
    /*
     * the constructor takes initializes the list with the given window size
     */
    DirectoryNavigator(WINDOW* statuswindow, int windowH, int windowW);

    virtual ~DirectoryNavigator();

    /*
     * updates the file list with a new dir on navigate "forwards" or "backwards"
     */
    void UpdateFileList(string dir);


    /*
     * this function is used when the same folder view is updated for a new highlight row.
     */
    void PrintFileList(int highlight, WINDOW *window);

    /*
     * this function is used when a new folder view is updated. ie: when going back
     * to the previous directory
     */
    void PrintFileList(int highlight, WINDOW *window, int minimum, int maximum);

    void ViewFile(int line, string pwd);

    string SelectFile(string pwd, int line);

    void Delete(string pwd, int line);


    //virtual string  GetSelectedDirName(int line) = 0;
    //virtual string  GetSelectedSoftLink(int line) = 0;

    virtual void    PasteFiles(string pwd, vector<string>& files) = 0;
    virtual string    GetDeviceName() = 0;

    bool    IsDirectory(int line);
    bool    IsSoftLink(int line);
    bool    IsFile(int line);

    string  GetSelectedFileName(int line);

    int     GetFileListSize()   { return m_fileList.size(); }
    int     getMin()            { return m_prevMin; }
    int     getMax()            { return m_prevMax; }
    void    setActivated(bool state) { m_isActive = state; }
    void    ClearSelectedFileList() { m_selectedFileList.clear(); }

protected:

    vector<FileData>    m_fileList;
    vector<int>         m_selectedFileList;
    WINDOW*             m_statWindow;
    int                 m_prevMin;
    int                 m_prevMax;
    unsigned int        m_windowH;
    unsigned int        m_windowW;
    bool                m_isActive;

    vector<string>      splitLine(string& input);
    FileData            simplifyLine(string& input);
    virtual void        TokenizeLine(string& input, FileData* fileData) = 0;
    virtual string      GetCommandString() = 0;
    virtual void        OpenFile(string pwd, string filename) = 0;
    virtual void        DeleteFile(string filename) = 0;

    string              ReplaceSpaces(string input);
    string NumberToString (double Number );

};


