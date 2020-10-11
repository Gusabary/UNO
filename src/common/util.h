#pragma once

#include "common.h"

namespace UNO { namespace Common {

class Util {
public:
    static int WrapWithPlayerNum(int numToWrap) {
        int ret = numToWrap % Common::mPlayerNum;
        if (ret < 0) {
            ret += Common::mPlayerNum;
        }
        return ret;
    }
};

}}