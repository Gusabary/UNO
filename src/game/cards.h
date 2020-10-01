#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <set>
#include <algorithm>

namespace UNO { namespace Game {

enum class CardColor : uint8_t {
    RED, YELLOW, GREEN, BLUE, BLACK
};
std::ostream& operator<<(std::ostream& os, const CardColor& color);

enum class CardText : uint8_t {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, 
    SKIP, REVERSE, DRAW_TWO, WILD, DRAW_FOUR, EMPTY
    /**
     * EMPTY is for conditions like:
     *   1. in the game start, the flipped card is yellow +2,
     *      now for the first player, the last played card is `yellow empty`,
     *      which means he can only play a yellow or wild card
     *   2. player A played a yellow +2 / +4, player B got the draw,
     *      now for player C, the last played card is `yellow empty`
     */
};

struct Card {
    CardColor mColor;
    CardText mText;

    Card() {}
    Card(const char *str);
    Card(CardColor color, CardText text) : mColor(color), mText(text) {}

    bool CanBePlayerAfter(Card lastPlayedCard, bool isUno);

    static CardColor FromChar(char c);

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};
inline bool operator==(const Card &lhs, const Card &rhs) {
    return (lhs.mColor == rhs.mColor) && (lhs.mText == rhs.mText);
}

class HandCards {
public:
    HandCards() {}

    void GetInitHandCards(const std::array<Card, 7> &cards);

    void Draw(const std::vector<Card> &cards);

    bool Play(int index, Card lastPlayedCard);

    bool Empty() const { return mCards.empty(); }

    Card At(int index) const { return mCards[index]; }

    friend std::ostream& operator<<(std::ostream& os, const HandCards& handCards);

private:
    std::vector<Card> mCards;
};
}}