#include "outputter.h"

namespace UNO { namespace UI {

const std::string ColorEscape::RESET  = "\033[0m";
const std::string ColorEscape::RED    = "\033[31m";
const std::string ColorEscape::YELLOW = "\033[33m";
const std::string ColorEscape::GREEN  = "\033[32m";
const std::string ColorEscape::BLUE   = "\033[34m";
// const std::string ColorEscape::BLACK  = "\033[30m";

Outputter::Outputter(std::unique_ptr<GameStat> &gameStat, 
    std::vector<PlayerStat> &playerStats, std::unique_ptr<HandCards> &handCards) 
    : mGameStat(gameStat), mPlayerStats(playerStats), mHandCards(handCards) 
{}

void Outputter::PrintRawView(const View &view) const
{
    auto [height, width] = ViewFormatter::GetBaseScaleOfView();
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            std::cout << view.At(row, col);
        }
        std::cout << std::endl;
    }
}

void Outputter::PrintView(const View &view) const
{
    auto [baseHeight, width] = ViewFormatter::GetBaseScaleOfView();
    int height = baseHeight + view.GetExtraRowNum();

    auto cardsToRender = GetCardsToRender();
    auto posesToRender = GetPosesToRender();
    int curRenderIndex = 0;
    int charsLeftToReset = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (row == posesToRender[curRenderIndex].first &&
                col == posesToRender[curRenderIndex].second) {
                std::cout << ToColorEscape(cardsToRender[curRenderIndex].mColor);
                charsLeftToReset = cardsToRender[curRenderIndex].Length();
                curRenderIndex++;
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
        std::cout << "Specify the next color (R/Y/G/B): ";
        // without std::endl, so a flush is needed
        std::cout.flush();
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

std::vector<Card> Outputter::GetCardsToRender() const
{
    std::vector<Card> cards;
    for (int i = 1; i < Common::Common::mPlayerNum; i++) {
        cards.emplace_back(mPlayerStats[i].GetLastPlayedCard());
    }
    cards.emplace_back(mGameStat->GetLastPlayedCard());
    for (int i = 0; i < mHandCards->Number(); i++) {
        cards.emplace_back(mHandCards->At(i));
    }
    return cards;
}

std::vector<ViewFormatter::PosT> Outputter::GetPosesToRender() const
{
    std::vector<ViewFormatter::PosT> poses;
    for (int i = 1; i < Common::Common::mPlayerNum; i++) {
        poses.emplace_back(ViewFormatter::GetPosOfPlayerLastPlayedCard(i));
    }
    poses.emplace_back(ViewFormatter::GetPosOfLastPlayedCard());
    for (int i = 0; i < mHandCards->Number(); i++) {
        poses.emplace_back(ViewFormatter::GetPosOfHandCard(i, *mHandCards));
    }
    return poses;
}

}}