#pragma once

#include "common.h"

namespace UNO { namespace Common {

class Util {
public:
    static int Wrap(int numToWrap, int range);

    static int WrapWithPlayerNum(int numToWrap);

    static char GetCharImmediately();
};

}}