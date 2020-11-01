#pragma once

#include <termios.h>
#include <unistd.h>

namespace UNO { namespace Common {

class Terminal {
public:
    Terminal();

    ~Terminal();

    void SetModeAutoFlush();

    void SetModeNoEcho();

    void Recover();

    static void DisableInput();
    
    static void EnableInput();

private:
    void GetNewAttr();

    void ApplyNewAttr();

private:
    struct termios mNewAttr;
    struct termios mOldAttr;
};
}}