#pragma once

#include "common.h"

namespace UNO { namespace Common {

class Util {
public:
    static int Wrap(int numToWrap, int range);

    static int WrapWithPlayerNum(int numToWrap);

    static char GetCharImmediately();

    static char GetCharWithTimeout(int milliseconds, bool autoFlush);

    static int GetSegmentNum(int handcardNum);

    static int GetSegmentIndex(int handcardIndex);

    static int GetIndexInSegment(int handcardIndex);
};
}}