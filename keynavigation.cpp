#include "keynavigation.h"

KeyHandler::KeyHandler(WINDOW *statuswindow, WINDOW* mainwindow, int windowH, int windowW, DeviceType device)
    : m_windowH(windowH),
      m_windowW(windowW),
      m_currentSelection(0),
      m_mainWindow(mainwindow),
      m_statusWindow(statuswindow)
{


    if(device == DeviceTypeAndroid)
    {
        m_strPwd = "/";
        m_DirectoryNavigator = new AdbNavigator(statuswindow, windowH, windowW);
    }
    else
    {
        m_strPwd = "~/";
        m_DirectoryNavigator = new PcNavigator(statuswindow, windowH, windowW);
    }



    m_DirectoryNavigator->UpdateFileList(m_strPwd);
    m_DirectoryNavigator->PrintFileList(m_currentSelection, m_mainWindow);
}

KeyHandler::~KeyHandler()
{
    delete m_DirectoryNavigator;
    m_DirectoryNavigator = NULL;
}

void KeyHandler::Backward()
{
    if(m_stCurrentFolder.size() > 0)
    {
        wclear(m_mainWindow);

        History& h = m_stCurrentFolder.top();

        int minimum = h.minimum;
        int maximum = h.maximum;
        m_strPwd = h.path;
        m_currentSelection = h.selection;

        m_stCurrentFolder.pop();

        m_DirectoryNavigator->UpdateFileList(m_strPwd);
        m_DirectoryNavigator->PrintFileList(m_currentSelection, m_mainWindow, minimum, maximum);

        wrefresh(m_mainWindow);
    }
}


void KeyHandler::Forward()
{

    if(m_DirectoryNavigator->IsDirectory(m_currentSelection) || m_DirectoryNavigator->IsSoftLink(m_currentSelection))
    {
        wclear(m_mainWindow);
        m_stCurrentFolder.push(History(m_strPwd, m_currentSelection, m_DirectoryNavigator->getMin(), m_DirectoryNavigator->getMax()));

        m_strPwd += m_DirectoryNavigator->GetSelectedFileName(m_currentSelection) + "/";

        m_currentSelection = 0;
        m_DirectoryNavigator->UpdateFileList(m_strPwd);
        m_DirectoryNavigator->PrintFileList(m_currentSelection, m_mainWindow);
        wrefresh(m_mainWindow);
    }
}


void KeyHandler::Up()
{
    wclear(m_mainWindow);
    if(m_currentSelection > 0) m_currentSelection--;

    m_DirectoryNavigator->PrintFileList(m_currentSelection, m_mainWindow);

    wrefresh(m_mainWindow);
}


void KeyHandler::Down()
{
    wclear(m_mainWindow);
    if(m_currentSelection < m_DirectoryNavigator->GetFileListSize()-1) m_currentSelection++;

    m_DirectoryNavigator->PrintFileList(m_currentSelection, m_mainWindow);
    wrefresh(m_mainWindow);
}

void KeyHandler::RefreshFiles()
{
    wclear(m_mainWindow);
    m_currentSelection = 0;
    m_DirectoryNavigator->UpdateFileList(m_strPwd);
    m_DirectoryNavigator->PrintFileList(m_currentSelection, m_mainWindow);
    wrefresh(m_mainWindow);
}

void KeyHandler::RefreshView()
{
    wclear(m_mainWindow);
    m_DirectoryNavigator->PrintFileList(m_currentSelection, m_mainWindow);
    wrefresh(m_mainWindow);
}

void KeyHandler::Disable()
{
    m_DirectoryNavigator->setActivated(false);
    RefreshView();
}

void KeyHandler::Enable()
{
    m_DirectoryNavigator->setActivated(true);
    m_DirectoryNavigator->ClearSelectedFileList();
    RefreshView();
}


void KeyHandler::ViewFile()
{
    if(m_DirectoryNavigator->IsFile(m_currentSelection))
        m_DirectoryNavigator->ViewFile(m_currentSelection, m_strPwd);

    RefreshView();
}


string KeyHandler::SelectFile()
{
    string selectedFilename = "";

    if(m_DirectoryNavigator->IsFile(m_currentSelection))
         selectedFilename = m_DirectoryNavigator->SelectFile(m_strPwd, m_currentSelection);
    RefreshView();
    return selectedFilename;
}


void KeyHandler::PasteFiles(vector<string>& files)
{
    m_DirectoryNavigator->PasteFiles(m_strPwd, files);
    files.clear();
    RefreshFiles();
}

void KeyHandler::Delete()
{
    if(m_DirectoryNavigator->IsFile(m_currentSelection))
    {
        wattron(m_mainWindow, COLOR_PAIR(2));
        wprintw(m_mainWindow, "press 'Y' to confirm delete");
        wattroff(m_mainWindow, COLOR_PAIR(2));
        if(wgetch(m_mainWindow) == 0x79)
        {
            m_DirectoryNavigator->Delete(m_strPwd, m_currentSelection);
            RefreshFiles();
        }
        else
        {
            RefreshView();
        }

    }
}

string KeyHandler::GetDeviceName()
{
    return m_DirectoryNavigator->GetDeviceName();
}
