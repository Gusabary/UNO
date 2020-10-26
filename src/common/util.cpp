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

char Util::GetCharImmediately()
{
    /// XXX: only apply to linux
    struct termios newAttr, oldAttr;

    // save the old attr
    if (tcgetattr(STDIN_FILENO, &oldAttr) != 0) {
        return -1;
    }

    // init the new attr as raw
    // cfmakeraw(&newAttr);
    tcgetattr(STDIN_FILENO, &newAttr);
    newAttr.c_lflag &= ~ICANON;
    newAttr.c_lflag &= ~ECHO;

    // set the attr to new attr
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newAttr) != 0) {
        return -1;
    }

    // get a char immediately
    char c = getchar();

    // recover the old attr
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldAttr) != 0) {
        return -1;
    }

    return c;
}

char Util::GetCharWithTimeout(int milliseconds, bool autoFlush)
{
    /// XXX: only apply to linux
    struct termios newAttr, oldAttr;

    if (autoFlush) {
        // save the old attr
        if (tcgetattr(STDIN_FILENO, &oldAttr) != 0) {
            return -1;
        }

        // init the new attr as raw
        // cfmakeraw(&newAttr);
        tcgetattr(STDIN_FILENO, &newAttr);
        newAttr.c_lflag &= ~ICANON;
        newAttr.c_lflag &= ~ECHO;

        // set the attr to new attr
        if (tcsetattr(STDIN_FILENO, TCSANOW, &newAttr) != 0) {
            return -1;
        }
    }

    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    int ret = poll(&pfd, 1, milliseconds);

    if (ret == 0) {
        // recover the old attr
        if (autoFlush) {
            if (tcsetattr(STDIN_FILENO, TCSANOW, &oldAttr) != 0) {
                return -1;
            }
        }
        throw std::runtime_error("timeout");
    }
    else if (ret == 1) {
        // recover the old attr
        if (autoFlush) {
            if (tcsetattr(STDIN_FILENO, TCSANOW, &oldAttr) != 0) {
                return -1;
            }
        }
        char c = getchar();
        return c;
    }
}

}}