#include "view_formatter.h"

namespace UNO { namespace UI {

std::vector<std::vector<ViewFormatter::PosT>> ViewFormatter::mPosOfPlayerBox;
std::vector<ViewFormatter::PosT> ViewFormatter::mPosOfLastPlayedCard;
std::vector<ViewFormatter::ScaleT> ViewFormatter::mBaseScaleOfView;
std::vector<ViewFormatter::PosT> ViewFormatter::mPosOfUNOText;

void ViewFormatter::Init()
{
    // player num is 0
    mPosOfPlayerBox.emplace_back(std::vector<PosT>());
    mPosOfLastPlayedCard.emplace_back(PosT{});
    mBaseScaleOfView.emplace_back(ScaleT{});
    mPosOfUNOText.emplace_back(PosT{});

    // player num is 1
    mPosOfPlayerBox.emplace_back(std::vector<PosT>());
    mPosOfLastPlayedCard.emplace_back(PosT{});
    mBaseScaleOfView.emplace_back(ScaleT{});
    mPosOfUNOText.emplace_back(PosT{});
    
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
     *                      Y2  UNO!      
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
    mPosOfLastPlayedCard.emplace_back(PosT{8, 20});
    mBaseScaleOfView.emplace_back(ScaleT{16, 42});
    mPosOfUNOText.emplace_back(PosT{8, 24});

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
     *                            Y3  UNO!
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
    mPosOfLastPlayedCard.emplace_back(PosT{8, 26});
    mBaseScaleOfView.emplace_back(ScaleT{16, 54});
    mPosOfUNOText.emplace_back(PosT{8, 30});

    /**
     * player num is 4
     *                      +--------------------+
     *                      |         a2         |
     *                      +--------------------+
     *                      | cards remained: 6  |
     *                      | last played: R8    |
     *                      +--------------------+
     * 
     *  +--------------------+                  +--------------------+ 
     *  |         a2         |                  |         a2         | 
     *  +--------------------+                  +--------------------+ 
     *  | cards remained: 6  |        Y4        | cards remained: 6  | 
     *  | last played: R8    |       UNO!       | last played: R8    | 
     *  +--------------------+                  +--------------------+ 
     *  ======>                       
     * 
     *            +----------------------------------------+
     *            |                   a1                   |
     *            +----------------------------------------+
     *            |   R+2  R+2  Y2  W  B+2  G5  G+2  G+2   |
     *            +----------------------------------------+
     */
    mPosOfPlayerBox.emplace_back(std::vector<PosT>{
        PosT{15, 10}, PosT{7, 0}, PosT{0, 20}, PosT{7, 40}
    });
    mPosOfLastPlayedCard.emplace_back(PosT{10, 30});
    mBaseScaleOfView.emplace_back(ScaleT{21, 62});
    mPosOfUNOText.emplace_back(PosT{11, 29});
}

ViewFormatter::PosT ViewFormatter::GetPosOfPlayerBox(int player)
{
    return mPosOfPlayerBox[Common::Common::mPlayerNum][player];
}

ViewFormatter::PosT ViewFormatter::GetPosOfLastPlayedCard()
{
    return mPosOfLastPlayedCard[Common::Common::mPlayerNum];
}

ViewFormatter::PosT ViewFormatter::GetPosOfUNOText(char c)
{
    int offset = -1;
    switch (c) {
        case 'U': offset = 0; break;
        case 'N': offset = 1; break;
        case 'O': offset = 2; break;
        case '!': offset = 3; break;
        default: assert(0);
    }
    auto startPos = mPosOfUNOText[Common::Common::mPlayerNum];
    return PosT{startPos.first, startPos.second + offset};
}

ViewFormatter::PosT ViewFormatter::GetPosOfPlayerLastPlayedCard(int playerIndex)
{
    auto [row, col] = GetPosOfPlayerBox(playerIndex);
    return ViewFormatter::PosT{row + 4, col + 15};
}

ViewFormatter::PosT ViewFormatter::GetPosOfHandCard(int handcardIndex, 
    const Game::HandCards &handcards)
{
    int segIndex = Common::Util::GetSegmentIndex(handcardIndex);
    int length = handcards.ToStringBySegment(segIndex).size();
    int indent = (42 - length) / 2;
    int indexInSeg = Common::Util::GetIndexInSegment(handcardIndex);
    auto [row, col] = GetPosOfPlayerBox(0);
    row += (3 + segIndex);
    col += (indent + handcards.LengthBeforeIndexInSegment(segIndex, indexInSeg) + 1);
    return ViewFormatter::PosT{row, col};
}

ViewFormatter::ScaleT ViewFormatter::GetBaseScaleOfView()
{
    return mBaseScaleOfView[Common::Common::mPlayerNum];
}

ViewFormatter::ScaleT ViewFormatter::GetMaxScaleOfView()
{
    auto [height, width] = GetBaseScaleOfView();
    return ViewFormatter::ScaleT{height + 6, width};
}

ViewFormatter::ScaleT ViewFormatter::GetBaseScaleOfBox(int playerIndex)
{
    if (playerIndex == 0) {
        return ViewFormatter::ScaleT{5, 42};
    }
    else {
        return ViewFormatter::ScaleT{6, 22};
    }
}

}}