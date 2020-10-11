#pragma once

#include <string>
#include <iostream>

namespace UNO { namespace UI {

class Inputter {
public:
    Inputter() {}

    std::string GetAction();

    char SpecifyNextColor();

private:
};

}}