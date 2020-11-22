#include <gtest/gtest.h>
#include <memory>

#include "../../src/game/stat.h"

namespace UNO { namespace Test {

using namespace testing;
using namespace Game;

class CardFixture : public ::testing::Test {
public:
    CardFixture() {}

    void SetUp() {}

    void TearDown() {}
};

TEST_F(CardFixture, CanBePlayedAfter) {
    // special cards can not be played as the last one
    EXPECT_FALSE(Card{"RS"}.CanBePlayedAfter("R0", true));
    EXPECT_FALSE(Card{"YS"}.CanBePlayedAfter("GS", true));
    EXPECT_FALSE(Card{"BR"}.CanBePlayedAfter("B+2", true));
    EXPECT_FALSE(Card{"G+2"}.CanBePlayedAfter("B+2", true));
    EXPECT_FALSE(Card{"W"}.CanBePlayedAfter("R2", true));
    EXPECT_FALSE(Card{"+4"}.CanBePlayedAfter("Y6", true));

    // if the last played card is skip, you can only play a skip
    EXPECT_FALSE(Card{"+4"}.CanBePlayedAfter("RS"));
    EXPECT_FALSE(Card{"W"}.CanBePlayedAfter("GS"));
    EXPECT_FALSE(Card{"R2"}.CanBePlayedAfter("YS"));
    EXPECT_TRUE(Card{"YS"}.CanBePlayedAfter("BS"));

    // if the last played card is draw two, you can only play a draw two or draw four
    EXPECT_FALSE(Card{"R2"}.CanBePlayedAfter("Y+2"));
    EXPECT_FALSE(Card{"W"}.CanBePlayedAfter("B+2"));
    EXPECT_TRUE(Card{"Y+2"}.CanBePlayedAfter("G+2"));
    EXPECT_TRUE(Card{"+4"}.CanBePlayedAfter("R+2"));

    // if the last played card is draw four, you can only play a draw four
    EXPECT_FALSE(Card{"R9"}.CanBePlayedAfter("R+4"));
    EXPECT_FALSE(Card{"W"}.CanBePlayedAfter("G+4"));
    EXPECT_FALSE(Card{"Y+2"}.CanBePlayedAfter("Y+4"));
    EXPECT_TRUE(Card{"+4"}.CanBePlayedAfter("B+4"));

    // wild card can always be played except above conditions
    EXPECT_FALSE(Card{"W"}.CanBePlayedAfter("R0", true));
    EXPECT_FALSE(Card{"+4"}.CanBePlayedAfter("BS"));
    EXPECT_FALSE(Card{"W"}.CanBePlayedAfter("B+2"));
    EXPECT_FALSE(Card{"W"}.CanBePlayedAfter("G+4"));
    EXPECT_TRUE(Card{"W"}.CanBePlayedAfter("RR"));
    EXPECT_TRUE(Card{"+4"}.CanBePlayedAfter("G5"));
    EXPECT_TRUE(Card{"W"}.CanBePlayedAfter("Y0"));

    // if not wild card, only cards with the same num or color can be played
    EXPECT_FALSE(Card{"RS"}.CanBePlayedAfter("GR"));
    EXPECT_TRUE(Card{"RS"}.CanBePlayedAfter("RR"));
    EXPECT_TRUE(Card{"RS"}.CanBePlayedAfter("R0"));
    EXPECT_FALSE(Card{"BR"}.CanBePlayedAfter("Y2"));
    EXPECT_TRUE(Card{"BR"}.CanBePlayedAfter("BR"));
    EXPECT_TRUE(Card{"BR"}.CanBePlayedAfter("YR"));
    EXPECT_FALSE(Card{"G4"}.CanBePlayedAfter("Y6"));
    EXPECT_TRUE(Card{"G3"}.CanBePlayedAfter("Y3"));
    EXPECT_TRUE(Card{"G2"}.CanBePlayedAfter("G0"));
    EXPECT_FALSE(Card{"Y+2"}.CanBePlayedAfter("G6"));
    EXPECT_TRUE(Card{"Y0"}.CanBePlayedAfter("Y3"));
    EXPECT_TRUE(Card{"Y9"}.CanBePlayedAfter("G9"));

    // handcards
    HandCards cards{{"Y0", "W", "RS", "BR", "Y+2", "G3", "+4"}};
    EXPECT_FALSE(cards.CanBePlayedAfter(cards.GetIndex("Y0"), "G8"));
    EXPECT_TRUE(cards.CanBePlayedAfter(cards.GetIndex("BR"), "B5"));
    EXPECT_TRUE(cards.CanBePlayedAfter(cards.GetIndex("W"), "G3"));
    EXPECT_FALSE(cards.CanBePlayedAfter(cards.GetIndex("Y+2"), "R0"));
    EXPECT_FALSE(cards.CanBePlayedAfter(cards.GetIndex("+4"), "RS"));
    EXPECT_TRUE(cards.CanBePlayedAfter(cards.GetIndex("+4"), "G+2"));
}

TEST_F(CardFixture, Length) {
    // single card
    EXPECT_EQ(Card{"R2"}.Length(), 2);
    EXPECT_EQ(Card{"Y+2"}.Length(), 3);
    EXPECT_EQ(Card{"W"}.Length(), 1);
    EXPECT_EQ(Card{"+4"}.Length(), 2);

    // handcards
    HandCards cards{{"RS", "Y0", "Y+2", "G3", "BR", "W", "+4"}};
    EXPECT_EQ(cards.LengthBeforeIndex(0), 0);
    EXPECT_EQ(cards.LengthBeforeIndex(1), 4);
    EXPECT_EQ(cards.LengthBeforeIndex(2), 8);
    EXPECT_EQ(cards.LengthBeforeIndex(3), 13);
    EXPECT_EQ(cards.LengthBeforeIndex(4), 17);
    EXPECT_EQ(cards.LengthBeforeIndex(5), 21);
    EXPECT_EQ(cards.LengthBeforeIndex(6), 24);
    EXPECT_EQ(cards.LengthBeforeIndex(7), 28);
    EXPECT_EQ(cards.Length(), 28);

    cards.Draw({"R4", "W"});
    EXPECT_EQ(cards.Length(), 35);

    cards.Erase(cards.GetIndex("+4"));
    EXPECT_EQ(cards.Length(), 31);

    cards.Erase(cards.GetIndex("W"));
    EXPECT_EQ(cards.Length(), 28);
}

TEST_F(CardFixture, Pile) {
    DiscardPile discardPile;
    Deck deck{discardPile};
    
    deck.Init();
    EXPECT_EQ(deck.GetPile().size(), 108);

    auto initHandCards = deck.DealInitHandCards(3);
    EXPECT_EQ(initHandCards.size(), 3);
    EXPECT_EQ(deck.GetPile().size(), 87);

    auto cards = deck.Draw(87);
    EXPECT_EQ(cards.size(), 87);
    EXPECT_EQ(deck.GetPile().size(), 0);
    EXPECT_EQ(discardPile.GetPile().size(), 0);

    deck.PutToBottom("+4");
    EXPECT_EQ(deck.GetPile().size(), 1);
    EXPECT_EQ(discardPile.GetPile().size(), 0);

    std::set<Card> cardset{"R0", "G+2", "BS", "YR", "W", "+4"};
    for (auto card : cardset) {
        discardPile.Add(card);
    }

    EXPECT_EQ(deck.GetPile().size(), 1);
    EXPECT_EQ(discardPile.GetPile().size(), 6);

    cards = deck.Draw(2);
    EXPECT_EQ(cards.size(), 2);
    EXPECT_TRUE(cardset.count(cards[0]));
    EXPECT_TRUE(cardset.count(cards[1]));
    EXPECT_EQ(deck.GetPile().size(), 5);
    EXPECT_EQ(discardPile.GetPile().size(), 0);
}
}}