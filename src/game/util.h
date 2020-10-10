#pragma once

namespace UNO { namespace Game {

class Util {
public:
    static int WrapWithPlayerNum(int numToWrap, int playerNum) {
        int ret = numToWrap % playerNum;
        if (ret < 0) {
            ret += playerNum;
        }
        return ret;
    }
};

}}