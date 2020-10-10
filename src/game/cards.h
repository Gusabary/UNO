#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <set>
#include <deque>
#include <algorithm>
#include <random>

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

    std::string ToString() const;

    int Length() const;

    static CardColor FromChar(char c);

    const static std::initializer_list<CardColor> NonWildColors;
    const static std::initializer_list<CardText> NonWildTexts;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};
inline bool operator==(const Card &lhs, const Card &rhs) {
    return (lhs.mColor == rhs.mColor) && (lhs.mText == rhs.mText);
}

class HandCards {
public:
    HandCards(const std::array<Card, 7> &cards);

    void Draw(const std::vector<Card> &cards);

    bool Play(int index, Card lastPlayedCard);

    bool Empty() const { return mCards.empty(); }

    Card At(int index) const { return mCards[index]; }

    std::string ToString() const;

    int Length() const;

    friend std::ostream& operator<<(std::ostream& os, const HandCards& handCards);

private:
    std::vector<Card> mCards;
};

/**
 * \c CardPile: a plie of cards, can be derived as \c Deck and \c DiscardPile
 * providing some methods about push/pop, which can be used in different scenarios:
 *   PushFront: Init deck in the game start. The card goes into discard pile.
 *   PopFront:  Draw from deck.
 *   PushBack:  When the flipped card is a wild card, put it back to under the deck.
 *   PopBack:   not used yet
 */
class CardPile {
protected:
    template <typename... Types>
    void PushFront(Types... args) {
        mPile.emplace_front(args...);
    }

    Card PopFront() {
        Card card = mPile.front();
        mPile.pop_front();
        return card;
    }

    template <typename... Types>
    void PushBack(Types... args) {
        mPile.emplace_back(args...);
    }

    Card PopBack() {
        Card card = mPile.back();
        mPile.pop_back();
        return card;
    }

    void Shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(mPile.begin(), mPile.end(), g);
    }

    void Swap(CardPile &pile) { std::swap(mPile, pile.mPile); }

    bool Empty() const { return mPile.empty(); }

private:
    std::deque<Card> mPile;
};

class DiscardPile : public CardPile {
public:
    void Add(Card card) { PushFront(card); }
};

class Deck : public CardPile {
public:
    Deck(DiscardPile &discardPile) : mDiscardPile(discardPile) {}

    void Init();

    std::vector<std::array<Card, 7>> DealInitHandCards(int playerNum);

    Card Draw();

    void PutToBottom(Card card) { PushBack(card); }
private:
    DiscardPile &mDiscardPile;
};

}}