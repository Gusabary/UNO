#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>

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
    return 0;
}
}}