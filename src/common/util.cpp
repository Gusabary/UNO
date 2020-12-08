#include <iostream>

#if defined(__unix__) || defined(__APPLE__)
#include <poll.h>
#elif defined(_WIN32)
#include <conio.h>
#include <windows.h>
#endif

#include "util.h"

namespace UNO { namespace Common {

int Util::Wrap(int numToWrap, int range)
{
    int ret = numToWrap % range;
    if (ret < 0) {
        ret += range;
    }
    return ret;
}

int Util::WrapWithPlayerNum(int numToWrap)
{
    return Wrap(numToWrap, Common::mPlayerNum);
}

int Util::GetSegmentNum(int handcardNum) { return (handcardNum - 1) / Common::mHandCardsNumPerRow + 1; }

int Util::GetSegmentIndex(int handcardIndex) { return handcardIndex / Common::mHandCardsNumPerRow; }

int Util::GetIndexInSegment(int handcardIndex) { return handcardIndex % Common::mHandCardsNumPerRow; }

char Util::GetCharWithTimeout(int milliseconds, bool autoFlush)
{
#if defined(__unix__) || defined(__APPLE__)
    std::unique_ptr<Terminal> terminal;
    if (autoFlush) {
        terminal.reset(new Terminal());
        terminal->SetModeAutoFlush();
    }

    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    int ret = poll(&pfd, 1, milliseconds);

    if (ret == 0) {
        throw std::runtime_error("timeout");
    }
    else if (ret == 1) {
        char c = getchar();
        return c;
    }
#elif defined(_WIN32)
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    auto ret = WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), milliseconds);

    if (ret == WAIT_TIMEOUT) {
        throw std::runtime_error("timeout");
    }
    else if (ret == WAIT_OBJECT_0) {
        char c = _getch();
        return c;
    }
#endif
    return 0;
}

void Util::HideTerminalCursor()
{
#if defined(_WIN32)
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cinfo;
    cinfo.bVisible = 0;
    cinfo.dwSize = 1;
    SetConsoleCursorInfo(hOutput, &cinfo);
#elif defined(__unix__) || defined(__APPLE__)
    // the hidden terminal cursor won't recover automatically,
    // so it's not a good choice to do that on Linux
    // std::cout << "\033[?25l" << std::endl;
#endif
}

}}