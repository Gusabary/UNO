#pragma once

#ifdef __unix__
#include <termios.h>
#include <unistd.h>
#endif

namespace UNO { namespace Common {

class Terminal {
public:
#ifdef __unix__
    /**
     * Save the old terminal mode.
     */
    Terminal();

    /**
     * Recover the old terminal mode. RAII.
     */
    ~Terminal();

    /**
     * Change the terminal mode so that input will auto flush 
     * (i.e. no longer need a '\n')
     */
    void SetModeAutoFlush();

    /**
     * Change the terminal mode so that input will not echo in the console.
     */
    void SetModeNoEcho();

    /**
     * Recover the old terminal explicitly.
     */
    void Recover();
#endif

    /**
     * Clear those chars that have been inputted but not consumed yet in the input buffer.
     */
    static void ClearStdInBuffer();

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