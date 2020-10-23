#include "view_formatter.h"

namespace UNO { namespace UI {

ViewFormatter::ViewFormatter()
{
    // player num is 0
    mPosOfPlayerBox.emplace_back(std::vector<PosT>());

    // player num is 1
    mPosOfPlayerBox.emplace_back(std::vector<PosT>());
    
    /** 
     * player num is 2
     *            +--------------------+ 
     *            |         a2         | 
     *            +--------------------+ 
     *            | cards remained: 6  | 
     *            | last played: R8    | 
     *            +--------------------+ 
     *            ======>             
     *  
     *                      Y2        
     *                            
     *  +----------------------------------------+
     *  |                   a1                   |
     *  +----------------------------------------+
     *  |   R+2  R+2  Y2  W  B+2  G5  G+2  G+2   |
     *  +----------------------------------------+
     */
    mPosOfPlayerBox.emplace_back(std::vector<PosT>{
        PosT{10, 0}, PosT{0, 10}
    });

    /**
     * player num is 3
     *  +--------------------+          +--------------------+ 
     *  |         a2         |          |         a2         | 
     *  +--------------------+          +--------------------+ 
     *  | cards remained: 6  |          | cards remained: 6  | 
     *  | last played: R8    |          | last played: R8    | 
     *  +--------------------+          +--------------------+ 
     *  ======>             
     *                            
     *                            Y3    
     *                            
     *        +----------------------------------------+
     *        |                   a1                   |
     *        +----------------------------------------+
     *        |   R+2  R+2  Y2  W  B+2  G5  G+2  G+2   |
     *        +----------------------------------------+
     */
    mPosOfPlayerBox.emplace_back(std::vector<PosT>{
        PosT{10, 6}, PosT{0, 0}, PosT{0, 32}
    });
}


ViewFormatter::PosT ViewFormatter::GetPosOfPlayerBox(int player) {
    return mPosOfPlayerBox[Common::Common::mPlayerNum][player];
}
}}