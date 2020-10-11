#include <termios.h>
#include <unistd.h>
#include <iostream>

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

char Util::GetCharImmediately()
{
    /// XXX: only apply to linux
    struct termios newAttr, oldAttr;

    // save the old attr
    if (tcgetattr(STDIN_FILENO, &oldAttr) != 0) {
        return -1;
    }

    // init the new attr as raw
    cfmakeraw(&newAttr);

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

}}