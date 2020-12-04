#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <set>
#include <deque>
#include <algorithm>
#include <random>
#ifdef ENABLE_LOG
#include <spdlog/spdlog.h>
#endif

#include "../common/common.h"

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
     *   3. player A played a green skip, player B was skipped,
     *      now for player C, the last played card is `green empty`
     */
};

/**
 * Some special sets of \c CardColor or \c CardText.
 */
struct CardSet {
    const static std::set<CardColor> NonWildColors;
    const static std::set<CardText> NonWildTexts;
    const static std::set<CardText> DrawTexts;
};

struct Card {
    CardColor mColor;
    CardText mText;

    Card() {}
    Card(const char *str);
    Card(CardColor color, CardText text) : mColor(color), mText(text) {}

    /**
     * Check whether a card can be played after another one.
     *   \param lastPlayedCard: the last card which is just played
     *   \param isUno: whether the card to play is the last (final) one in the player's handcard
     *   \return whether the card can played after the last played one
     */
    bool CanBePlayedAfter(Card lastPlayedCard, bool isUno = false);

    /**
     * Get the string format of a card, like 'W', 'R6', 'B+2'.
     */
    std::string ToString() const;

    /**
     * Get the length of the string format of the card, which can be 1, 2, or 3.
     */
    int Length() const;

    static CardColor FromChar(char c);

    bool operator<(const Card &rhs) const {
        return (mColor < rhs.mColor) || 
            (mColor == rhs.mColor && mText < rhs.mText);
    }

    bool operator==(const Card &rhs) const {
        return (mColor == rhs.mColor) && (mText == rhs.mText);
    }

    bool operator!=(const Card &card) const {
        return !(*this == card);
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};

class HandCards {
public:
    HandCards(const std::array<Card, 7> &cards);

    /**
     * Draw a list of cards and add them into handcards.
     */
    void Draw(const std::vector<Card> &cards);

    /**
     * Check whether the card positioned at \param index in the handcard 
     * can be played after the \param lastPlayedCard.
     */
    bool CanBePlayedAfter(int index, Card lastPlayedCard);

    /**
     * Remove the card positioned at \param index in the handcard.
     */
    void Erase(int index);

    /**
     * Check whether there is any card in the handcard.
     */
    bool Empty() const { return mCards.empty(); }

    /**
     * Get the index of a given card in the handcards. Noth that:
     *   1) the given card has to exist in the handcards (guaranteed by the caller)
     *   2) if the given card is duplicated in the handcards, return the index of the first one
     */
    int GetIndex(Card card) const;

    /**
     * Get the card positioned at \param index in the handcard.
     */
    Card At(int index) const {
        return *IteratorAt(index);
    }

    /**
     * Get the number of cards in the handcard.
     */
    int Number() const { return mCards.size(); }

    /**
     * Get the string format of the entire handcard.
     */
    std::string ToString() const;

    /**
     * Get the string format of the part of handcard, whcih is in segment \param seg.
     */
    std::string ToStringBySegment(int seg) const;

    /**
     * Get the length of the string format of the entire handcard, 
     * including the ' ' between adjacent cards and on both sides.
     */
    int Length() const;

    /**
     * Get the length of the string format of the part of handcard,
     * which is (exclusively) before the segment \param index
     */
    int LengthBeforeIndex(int index) const;

    /**
     * Get the length of the string format of the part of handcard,
     * which belongs to segment \param segIndex and before card \param indexInSeg.
     */
    int LengthBeforeIndexInSegment(int segIndex, int indexInSeg) const;

    /**
     * Compare the handcards before and after drawing a card, return the index of the newly drawn.
     */
    int GetIndexOfNewlyDrawn(const HandCards &handcardsBeforeDraw) const;

    friend std::ostream& operator<<(std::ostream& os, const HandCards& handCards);

private:
    std::multiset<Card>::iterator IteratorAt(int index) const;

    std::string ToStringByCard(int start, int len) const;

private:
    // use multiset to ensure that handcards are always in order
    std::multiset<Card> mCards;
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

    void Clear() { mPile.clear(); }

    bool Empty() const { return mPile.empty(); }

public:
    // for test
    std::deque<Card> GetPile() const { return mPile; }

private:
    std::deque<Card> mPile;
};

class DiscardPile : public CardPile {
public:
    /**
     * Add a card to discard pile.
     */
    void Add(Card card) { PushFront(card); }

    /**
     * Clear the discard pile.
     */
    void Clear() { CardPile::Clear(); }
};

class Deck : public CardPile {
public:
    Deck(DiscardPile &discardPile) : mDiscardPile(discardPile) {}

    /**
     * Init deck with the 108 UNO cards and shuffle.
     */
    void Init();

    /**
     * Deal 7 cards to each player as the initial handcards.
     */
    std::vector<std::array<Card, 7>> DealInitHandCards(int playerNum);

    /**
     * Draw a card from deck.
     */
    Card Draw();

    /**
     * Draw \param number cards from deck.
     */
    std::vector<Card> Draw(int number);

    /**
     * Put a card to the bottom of deck.
     */
    void PutToBottom(Card card) { PushBack(card); }
    
private:
    // link a discard pile to deck. when the deck is exhausted, swap them
    DiscardPile &mDiscardPile;
};

}}