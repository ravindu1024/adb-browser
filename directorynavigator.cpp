#include "directorynavigator.h"
#include "stdio.h"
#include <iomanip>
#include <cstring>
#include <string>

DirectoryNavigator::DirectoryNavigator(WINDOW* statuswindow, int windowH, int windowW)
    : m_statWindow(statuswindow),
      m_prevMin(0),
      m_prevMax(windowH),
      m_windowH(windowH),
      m_windowW(windowW),
      m_isActive(true)
{
    m_fileList.clear();
}

DirectoryNavigator::~DirectoryNavigator()
{

}

void DirectoryNavigator::UpdateFileList(string dir)
{
    string command = GetCommandString() + dir;

    FILE* fp = popen(command.c_str(), "r");

    char buff[1024] = "";

    if(fp != NULL)
    {
        m_fileList.clear();
        m_selectedFileList.clear();
        int lineCount = 0;

        while(fgets(buff, sizeof(buff), fp) != NULL)
        {

            lineCount++;
            string line(buff);

            if(lineCount == 1)
            {
                if(strncmp(buff, "total", 5) == 0)
                    continue;
            }

            FileData t = simplifyLine(line);

            m_fileList.push_back(t);
        }
    }

    m_prevMax = (m_fileList.size() > m_windowH)? m_windowH : m_fileList.size();
    m_prevMin = 0;


    wmove(m_statWindow, 1, 1);
    wclrtoeol(m_statWindow);
    wmove(m_statWindow, 2, 0);
    wclrtoeol(m_statWindow);
    mvwprintw(m_statWindow, 1, 1, "%s", dir.c_str());
    wrefresh(m_statWindow);

    pclose(fp);
}

void DirectoryNavigator::PrintFileList(int highlight, WINDOW *window)
{

    PrintFileList(highlight, window, m_prevMin, m_prevMax);
}

void DirectoryNavigator::PrintFileList(int highlight, WINDOW* window, int minimum, int maximum)
{
    if(!m_fileList.empty())
    {
        if(highlight > maximum-1)
        {
            minimum++;
            maximum++;
        }
        if(highlight < minimum)
        {
            minimum--;
            maximum--;
        }

        init_pair(2, COLOR_RED, COLOR_BLACK);


        for(int i = minimum; i<maximum; i++)
        {

            if(highlight == i && m_isActive)
                wattron(window, A_STANDOUT);

            const FileData& fd = m_fileList.at(i);
            ostringstream os;
            string filename = fd.filename;
            u_int32_t maxNameLen = m_windowW-(PERMISSIONS_WIDTH+USERNAME_WIDTH+1);
            if(filename.size() > maxNameLen-1)
            {
                filename.resize(maxNameLen-2);
            }
            filename += " ";

            os << left << setw(m_windowW-(PERMISSIONS_WIDTH+USERNAME_WIDTH+1)) << filename;
            os << right << setw(USERNAME_WIDTH) << (fd.size + " ");
            os << right << setw(PERMISSIONS_WIDTH) << fd.permissions <<  endl;

            string displayLine = os.str();

//            if(displayLine.size() > (m_windowW))
//            {
//                displayLine.resize(m_windowW-1);
//                displayLine += "\n";
//            }

            if(std::find(m_selectedFileList.begin(), m_selectedFileList.end(), i) != m_selectedFileList.end())
            {
                wattron(window, COLOR_PAIR(2));
            }

            wprintw(window, displayLine.c_str());

            if(highlight == i && m_isActive)
                wattroff(window, A_STANDOUT);
            if(std::find(m_selectedFileList.begin(), m_selectedFileList.end(), i) != m_selectedFileList.end())
            {
                wattroff(window, COLOR_PAIR(2));
            }
        }

        m_prevMax = maximum;
        m_prevMin = minimum;

    }
}



FileData DirectoryNavigator::simplifyLine(std::string &input)
{

    FileData ret;

    input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());

    TokenizeLine(input, &ret);

    return ret;
}


string DirectoryNavigator::GetSelectedFileName(int line)
{
    if(!m_fileList.empty())
    {
        return m_fileList[line].formattedFilename;
    }
    else return "";
}

string DirectoryNavigator::NumberToString(double Number)
{
     ostringstream ss;
     ss << std::fixed << std::setprecision(1) << Number;
     return ss.str();
}

vector<string> DirectoryNavigator::splitLine(string &input)
{
    std::vector<std::string> tokens;
    std::vector<string> ret;
    stringstream iss(input);

    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(tokens));

    return tokens;
}



bool DirectoryNavigator::IsDirectory(int line)
{
    if(m_fileList.empty()) return false;

    if (m_fileList[line].permissions.compare(0, 1, "d") == 0)
        return true;
    else
        return false;
}

bool DirectoryNavigator::IsSoftLink(int line)
{
    if(m_fileList.empty()) return false;
    if (m_fileList[line].permissions.compare(0, 1, "l") == 0)
        return true;
    else
        return false;
}

bool DirectoryNavigator::IsFile(int line)
{
    if(m_fileList.empty()) return false;
    if (m_fileList[line].permissions.compare(0, 1, "-") == 0)
        return true;
    else
        return false;
}

void DirectoryNavigator::ViewFile(int line, string pwd)
{
    string filename = m_fileList[line].formattedFilename;

    OpenFile(pwd, filename);
}



string DirectoryNavigator::ReplaceSpaces(string input)
{
    string delim = " ";
    for(u_int32_t i=0; i < input.size(); i++)
    {
        if(input.at(i) == ' ')
        {
            input.replace(i, delim.length(), "\\ ");
            i++;
        }
    }

    return input;
}


string DirectoryNavigator::SelectFile(string pwd, int line)
{
    m_selectedFileList.push_back(line);
    string filename = pwd + m_fileList[line].formattedFilename;
    return filename;
}

void DirectoryNavigator::Delete(string pwd, int line)
{
    string file = m_fileList[line].formattedFilename;
    file = pwd + file;

    DeleteFile(file);
}

