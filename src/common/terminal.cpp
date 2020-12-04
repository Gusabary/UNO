#ifdef _WIN32
#include <conio.h>
#endif
#include "terminal.h"

namespace UNO { namespace Common {

#if defined(__unix__) || defined(__APPLE__)
Terminal::Terminal()
{
    /// XXX: what if throwing an exception
    // save the old attr
    tcgetattr(STDIN_FILENO, &mOldAttr);
}

void Terminal::GetNewAttr()
{
    tcgetattr(STDIN_FILENO, &mNewAttr);
}

void Terminal::SetModeAutoFlush()
{
    GetNewAttr();
    mNewAttr.c_lflag &= ~ICANON;
    mNewAttr.c_lflag &= ~ECHO;
    ApplyNewAttr();
}

void Terminal::SetModeNoEcho()
{
    GetNewAttr();
    mNewAttr.c_lflag &= ~ECHO;
    ApplyNewAttr();
}

void Terminal::ApplyNewAttr()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &mNewAttr);
}

void Terminal::Recover()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &mOldAttr);
}

Terminal::~Terminal()
{
    Recover();
}
#endif

void Terminal::ClearStdInBuffer()
{
#if defined(__unix__) || defined(__APPLE__)
    tcflush(STDIN_FILENO, TCIFLUSH);
#elif defined(_WIN32)
    while (true) {
        auto ret = _kbhit();
        if (ret != 0) {
            _getch();
        }
        else {
            break;
        }
    }
#endif
}

}}