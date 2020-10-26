#include "view_formatter.h"

namespace UNO { namespace UI {

std::vector<std::vector<ViewFormatter::PosT>> ViewFormatter::mPosOfPlayerBox;
std::vector<ViewFormatter::PosT> ViewFormatter::mPosOfLastPlayedCard;
std::vector<ViewFormatter::ScaleT> ViewFormatter::mBaseScaleOfView;

void ViewFormatter::Init()
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

    mPosOfLastPlayedCard.emplace_back(PosT{});
    mPosOfLastPlayedCard.emplace_back(PosT{});
    mPosOfLastPlayedCard.emplace_back(PosT{8, 20});
    mPosOfLastPlayedCard.emplace_back(PosT{8, 26});

    mBaseScaleOfView.emplace_back(ScaleT{});
    mBaseScaleOfView.emplace_back(ScaleT{});
    mBaseScaleOfView.emplace_back(ScaleT{16, 42});
    mBaseScaleOfView.emplace_back(ScaleT{16, 54});
}

ViewFormatter::PosT ViewFormatter::GetPosOfPlayerBox(int player)
{
    return mPosOfPlayerBox[Common::Common::mPlayerNum][player];
}

ViewFormatter::PosT ViewFormatter::GetPosOfLastPlayedCard()
{
    return mPosOfLastPlayedCard[Common::Common::mPlayerNum];
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