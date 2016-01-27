#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <ncurses.h>
#include <sys/types.h>
#include <termios.h>
#include <stack>
#include <sys/ioctl.h>
#include "keynavigation.h"
#include "CommonDefines.h"



using namespace std;

void PrintHotKeys(WINDOW *window);
void PrintAbout(WINDOW *window);

int main()
{
    winsize         termSize;
    vector<string>  copiedFiles;

    WINDOW*         adbWindow           = NULL;
    WINDOW*         headerWindow        = NULL;
    WINDOW*         adbStatusWindow     = NULL;
    WINDOW*         pcWindow            = NULL;
    WINDOW*         pcStatusWindow      = NULL;
    bool            bKeepLoop           = true;
    bool            bDeviceActive       = true;
    int             inputKey            = 0;
    bool            isAboutDisplayed    = false;



    ioctl(0, TIOCGWINSZ, &termSize);
    if(termSize.ws_row < MIN_ROWS)
    {
        cout << "Insufficient terminal height" <<endl;
        return 0;
    }
    if(termSize.ws_col < MIN_COLUMNS)
    {
        cout << "Insufficient terminal width" <<endl;
        return 0;
    }


    cout << "ADB Browser version 1.0\n" << endl;


    initscr();
    clear();
    cbreak();    
    noecho();
    curs_set(0);
    start_color();


    /* Initialize windows */
    headerWindow    = newwin(HEADER_HEIGHT, termSize.ws_col, 0, 0);
    adbStatusWindow = newwin(STATUS_HEIGHT, termSize.ws_col/2-1, HEADER_HEIGHT, 0);
    pcStatusWindow  = newwin(STATUS_HEIGHT, termSize.ws_col/2-1, HEADER_HEIGHT, termSize.ws_col/2+1);
    adbWindow       = newwin(termSize.ws_row-(HEADER_HEIGHT+STATUS_HEIGHT+1), termSize.ws_col/2-1, STATUS_HEIGHT+HEADER_HEIGHT+1, 1);
    pcWindow        = newwin(termSize.ws_row-(HEADER_HEIGHT+STATUS_HEIGHT+1), termSize.ws_col/2-1, STATUS_HEIGHT+HEADER_HEIGHT+1, termSize.ws_col/2+1);


    /* set the keyboard shortcuts */
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    wattron(headerWindow, A_BOLD);

    box(headerWindow, 0, 0);
    mvwaddstr(headerWindow, 0, termSize.ws_col/2-10, "ADB Browser 1.0");

    wrefresh(headerWindow);
    wattroff(headerWindow, A_BOLD);


    /* print the keyboard shortcuts */
    PrintHotKeys(headerWindow);

    wrefresh(headerWindow);

    //whline(adbStatusWindow, '_', termSize.ws_col);
    box(adbStatusWindow, 0, 0);
    wrefresh(adbStatusWindow);

    //whline(pcStatusWindow, '_', termSize.ws_col);
    box(pcStatusWindow, 0, 0);
    wrefresh(pcStatusWindow);


    /* initialize key handlers */

    KeyHandler adbNavigation(adbStatusWindow, adbWindow, termSize.ws_row-WINDOW_MARGIN-1, termSize.ws_col/2-1, DeviceTypeAndroid);
    KeyHandler pcNavigation(pcStatusWindow, pcWindow, termSize.ws_row-WINDOW_MARGIN-1, termSize.ws_col/2-1, DeviceTypex86);


    /* display device names */
    mvwaddstr(adbStatusWindow, 0, 0, adbNavigation.GetDeviceName().c_str());
    mvwaddstr(pcStatusWindow, 0, 0, pcNavigation.GetDeviceName().c_str());



    adbNavigation.RefreshFiles();
    pcNavigation.Disable();
    pcNavigation.RefreshFiles();

    KeyHandler* selectedWindowBrowser = NULL;
    WINDOW* selectedWindow = NULL;



    /* Main key handling loop */
    while(bKeepLoop)
    {
        if(bDeviceActive)
        {
            selectedWindowBrowser = &adbNavigation;
            selectedWindow = adbWindow;
        }
        else
        {
            selectedWindowBrowser = &pcNavigation;
            selectedWindow = pcWindow;
        }

        inputKey = wgetch(selectedWindow);

        if(isAboutDisplayed)
        {
            isAboutDisplayed = false;
            PrintHotKeys(headerWindow);
        }

        switch(inputKey)
        {

            case VKEY_BACKSPACE:
            {
                selectedWindowBrowser->Backward();
            }
            break;

            case VKEY_ENTER:
            {
                selectedWindowBrowser->Forward();
            }
            break;

            case VKEY_Q:
            {
                bKeepLoop = false;
            }
            break;

            case VKEY_TAB:
            case VKEY_K:
            {
                if(bDeviceActive)
                {
                    adbNavigation.Disable();
                    pcNavigation.Enable();
                }
                else
                {
                    adbNavigation.Enable();
                    pcNavigation.Disable();
                }
                bDeviceActive = !bDeviceActive;
            }
            break;

            case VKEY_V:
            {
                selectedWindowBrowser->ViewFile();
            }
            break;

            case VKEY_R:
            {
                selectedWindowBrowser->RefreshFiles();
                copiedFiles.clear();
            }
            break;

            case VKEY_C:
            {
                string file = selectedWindowBrowser->SelectFile();
                if(file.size() > 0)
                {
                    copiedFiles.push_back(file);
                }
            }
            break;

            case VKEY_P:
            {
                selectedWindowBrowser->PasteFiles(copiedFiles);
            }
            break;
            case VKEY_A:
            {
                isAboutDisplayed = true;
                PrintAbout(headerWindow);
            }
            break;

            case VKEY_ESC_CHAR:
            {
                if(wgetch(selectedWindow) == VKEY_ESC_CHAR2)
                {
                    char ch2 = wgetch(selectedWindow);
                    switch(ch2)
                    {
                        case VKEY_ESC_CHAR3:
                            if(wgetch(selectedWindow) == VKEY_DELETE)
                            {
                                selectedWindowBrowser->Delete();
                            }
                        break;

                        case VKEY_DOWN:
                        {
                            selectedWindowBrowser->Down();
                        }
                        break;

                        case VKEY_UP:
                        {
                            selectedWindowBrowser->Up();
                        }
                        break;

                        case VKEY_RIGHT:
                        {
                            selectedWindowBrowser->Forward();
                        }
                        break;

                        case VKEY_LEFT:
                        {
                            selectedWindowBrowser->Backward();
                        }
                        break;
                    }
                }
            }
            break;

            default:
            {
                //debug only
//                wprintw(adbWindow, "invalid key: 0x%x", inputKey);
//                wrefresh(adbWindow);
            }
        }
    }

    delwin(adbWindow);
    delwin(headerWindow);
    delwin(adbStatusWindow);
    delwin(pcStatusWindow);
    delwin(pcWindow);

    /* End curses mode */
    endwin();

    return 0;
}

void PrintHotKeys(WINDOW* window)
{
    wmove(window, 1, 1);
    wclrtoeol(window);
    wmove(window, 2, 1);
    wclrtoeol(window);
    mvwprintw(window, 1, 3,  "Quit");
    mvwprintw(window, 1, 10, "'Q'");
    mvwprintw(window, 1, 20, "Copy");
    mvwprintw(window, 1, 30, "'C'");
    mvwprintw(window, 1, 40, "Switch Window");
    mvwprintw(window, 1, 55, "'TAB'");
    mvwprintw(window, 2, 3,  "Paste");
    mvwprintw(window, 2, 10, "'P'");
    mvwprintw(window, 2, 20, "View File 'V'");
    mvwprintw(window, 2, 40, "About");
    mvwprintw(window, 2, 55, "'A'");
    wrefresh(window);
}

void PrintAbout(WINDOW *window)
{
    wmove(window, 1, 1);
    wclrtoeol(window);
    wmove(window, 2, 1);
    wclrtoeol(window);
    wattron(window, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(window, 1, 3,  "Author  : Ravindu");
    mvwprintw(window, 2, 3,  "Version : 1.0.0");
    wattroff(window, COLOR_PAIR(1));
    wattron(window, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(window, 1, 25, "This application can be used to ");
    mvwprintw(window, 2, 25,  "browse/pull/push files from android devices");
    wattroff(window, COLOR_PAIR(2) | A_BOLD);
    wrefresh(window);
}
