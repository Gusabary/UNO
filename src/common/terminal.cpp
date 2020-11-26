#include "terminal.h"

namespace UNO { namespace Common {

#ifdef __unix__
Terminal::Terminal()
{
    /// XXX: what if throwing an exception
    /// XXX: how to apply to windows
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

void Terminal::DisableInput()
{
    /// TODO: how to disable keyboard input
}

void Terminal::EnableInput()
{
    /// TODO: how to enable keyboard input
}

}}