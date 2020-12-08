#ifdef _WIN32
#include <windows.h>
#endif

#include "outputter.h"

namespace UNO { namespace UI {

std::string ColorEscape::RESET;
std::string ColorEscape::RED;
std::string ColorEscape::YELLOW;
std::string ColorEscape::GREEN;
std::string ColorEscape::BLUE;
// const std::string ColorEscape::BLACK  = "\033[30m";

Outputter::Outputter(std::unique_ptr<GameStat> &gameStat, 
    std::vector<PlayerStat> &playerStats, std::unique_ptr<HandCards> &handCards) 
    : mGameStat(gameStat), mPlayerStats(playerStats), mHandCards(handCards) 
{
    ColorEscape::RESET  = "\033[0m";
    ColorEscape::RED    = Common::Common::mRedEscape;
    ColorEscape::YELLOW = Common::Common::mYellowEscape;
    ColorEscape::GREEN  = Common::Common::mGreenEscape;
    ColorEscape::BLUE   = Common::Common::mBlueEscape;
}

void Outputter::PrintRawView(const View &view) const
{
    ClearScreen();
    auto [height, width] = ViewFormatter::GetBaseScaleOfView();
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::cout << view.At(row, col);
        }
        std::cout << std::endl;
    }
}

void Outputter::PrintView(const View &view, bool useCls) const
{
    ClearScreen(useCls);
    auto [baseHeight, width] = ViewFormatter::GetBaseScaleOfView();
    int height = baseHeight + view.GetExtraRowNum();

    auto renderInfos = GetRenderInfos();
    int curRenderIndex = 0;
    int charsLeftToReset = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (curRenderIndex < renderInfos.size()) {
                auto curRenderInfo = renderInfos[curRenderIndex];
                if (row == curRenderInfo.mPos.first &&
                    col == curRenderInfo.mPos.second)
                {
                    std::cout << ToColorEscape(curRenderInfo.mCard.mColor);
                    charsLeftToReset = curRenderInfo.mCard.Length();
                    curRenderIndex++;
                }
            }
            std::cout << view.At(row, col);
            charsLeftToReset--;
            if (charsLeftToReset == 0) {
                std::cout << ColorEscape::RESET;
            }
        }
        std::cout << std::endl;
    }
}

void Outputter::PrintHintText(bool isSpecifyingNextColor, bool lastCardCanBePlayed,
    bool hasChanceToPlayAfterDraw) const
{
    if (isSpecifyingNextColor) {
        std::cout << "Specify the next color (" 
                  << ToColorEscape(CardColor::RED)    << "R" << ColorEscape::RESET << "/"
                  << ToColorEscape(CardColor::YELLOW) << "Y" << ColorEscape::RESET << "/"
                  << ToColorEscape(CardColor::GREEN)  << "G" << ColorEscape::RESET << "/"
                  << ToColorEscape(CardColor::BLUE)   << "B" << ColorEscape::RESET << ")." << std::endl;
    }
    else if (!lastCardCanBePlayed) {
        auto lastPlayedCard = mGameStat->GetLastPlayedCard();
        std::cout << "This card cannot be played. Last played card is "
                  << ToColorEscape(lastPlayedCard.mColor)
                  << lastPlayedCard << ColorEscape::RESET << std::endl;
        std::cout << "Press , and . to move the cursor and Enter to play the card." << std::endl;
        std::cout << "Or press Space to draw cards / skip." << std::endl;
    }
    else if (!hasChanceToPlayAfterDraw) {
        std::cout << "Now it's your turn." << std::endl;
        std::cout << "Press , and . to move the cursor and Enter to play the card." << std::endl;
        std::cout << "Or press Space to draw cards / skip." << std::endl;
    }
    else {
        std::cout << "Press Enter to play the card just drawn immediately." << std::endl;
        std::cout << "Or press Space to turn to the next player." << std::endl;
    }
}

std::string Outputter::ToColorEscape(CardColor color) const
{
    switch (color) {
        case CardColor::RED:    return ColorEscape::RED;
        case CardColor::YELLOW: return ColorEscape::YELLOW;
        case CardColor::GREEN:  return ColorEscape::GREEN;
        case CardColor::BLUE:   return ColorEscape::BLUE;
        // case CardColor::BLACK:  return ColorEscape::BLACK;
        default: return ColorEscape::RESET;
    }
}

std::vector<RenderInfo> Outputter::GetRenderInfos() const
{
    std::vector<RenderInfo> renderInfos;
    for (int i = 1; i < Common::Common::mPlayerNum; i++) {
        renderInfos.emplace_back(ViewFormatter::GetPosOfPlayerLastPlayedCard(i), 
            mPlayerStats[i].GetLastPlayedCard());
    }
    renderInfos.emplace_back(ViewFormatter::GetPosOfLastPlayedCard(), 
        mGameStat->GetLastPlayedCard());
    for (int i = 0; i < mHandCards->Number(); i++) {
        renderInfos.emplace_back(ViewFormatter::GetPosOfHandCard(i, *mHandCards), 
            mHandCards->At(i));
    }
    // 'U' is red, 'N' is yellow, 'O' is green and '!' is blue.
    // only color matters, text can be anything here
    renderInfos.emplace_back(ViewFormatter::GetPosOfUNOText('U'), 
        Game::Card(Game::CardColor::RED, Game::CardText::ZERO));
    renderInfos.emplace_back(ViewFormatter::GetPosOfUNOText('N'), 
        Game::Card(Game::CardColor::YELLOW, Game::CardText::ZERO));
    renderInfos.emplace_back(ViewFormatter::GetPosOfUNOText('O'), 
        Game::Card(Game::CardColor::GREEN, Game::CardText::ZERO));
    renderInfos.emplace_back(ViewFormatter::GetPosOfUNOText('!'), 
        Game::Card(Game::CardColor::BLUE, Game::CardText::ZERO));
    
    std::sort(renderInfos.begin(), renderInfos.end());
    return renderInfos;
}

void Outputter::ClearScreen(bool useCls) const
{
#ifdef _WIN32
    if (useCls) {
        system("cls");
    }
    else {
        // not use system('cls') for better player experience
        static HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        static COORD coord = {0, 0};
        SetConsoleCursorPosition(hOutput, coord);
    }
#else
    system("clear");
#endif
}

}}