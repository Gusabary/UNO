#include "msg.h"

namespace UNO { namespace Network {

std::ostream& operator<<(std::ostream& os, const ActionType& type)
{
    std::string typeStr;
    switch (type) {
        case ActionType::DRAW: typeStr = "DRAW"; break;
        case ActionType::SKIP: typeStr = "SKIP"; break;
        case ActionType::PLAY: typeStr = "PLAY"; break;
        default: assert(0);
    }

    os << typeStr;
    return os;
}
}}