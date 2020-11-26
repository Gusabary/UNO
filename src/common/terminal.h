#pragma once

#ifdef __unix__
#include <termios.h>
#include <unistd.h>
#endif

namespace UNO { namespace Common {

class Terminal {
public:
#ifdef __unix__
    Terminal();

    ~Terminal();

    void SetModeAutoFlush();

    void SetModeNoEcho();

    void Recover();
#endif

    static void DisableInput();
    
    static void EnableInput();

#ifdef __unix__
private:
    void GetNewAttr();

    void ApplyNewAttr();

private:
    struct termios mNewAttr;
    struct termios mOldAttr;
#endif
};
}}