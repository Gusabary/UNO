#include "cards.h"

namespace UNO { namespace Game {

const std::set<CardColor> CardSet::NonWildColors = 
    { CardColor::RED, CardColor::YELLOW, CardColor::GREEN, CardColor::BLUE };

const std::set<CardText> CardSet::NonWildTexts = 
    { CardText::ZERO, CardText::ONE, CardText::TWO, CardText::THREE, CardText::FOUR, 
      CardText::FIVE, CardText::SIX, CardText::SEVEN, CardText::EIGHT, CardText::NINE, 
      CardText::SKIP, CardText::REVERSE, CardText::DRAW_TWO };

const std::set<CardText> CardSet::DrawTexts = { CardText::DRAW_TWO, CardText::DRAW_FOUR };

HandCards::HandCards(const std::array<Card, 7> &cards)
{
    for (auto card : cards) {
        mCards.emplace(card);
    }
}

void HandCards::Draw(const std::vector<Card> &cards)
{
    std::for_each(cards.begin(), cards.end(), [this](const Card &card) {
        mCards.emplace(card);
    });
}

bool HandCards::CanBePlayedAfter(int index, Card lastPlayedCard)
{
    assert(index < mCards.size());
    Card cardToPlay = At(index);
    bool isUno = (mCards.size() == 1);
    return cardToPlay.CanBePlayedAfter(lastPlayedCard, isUno);
}

void HandCards::Erase(int index)
{
    mCards.erase(std::next(mCards.begin(), index));
}

int HandCards::GetIndex(Card card) const
{
    auto it = mCards.find(card);
    assert(it != mCards.end());
    return std::distance(mCards.begin(), it);
}

std::multiset<Card>::iterator HandCards::IteratorAt(int index) const {
    auto it = std::begin(mCards);
    std::advance(it, index);
    return it;
}

int HandCards::GetIndexOfNewlyDrawn(const HandCards &handcardsBeforeDraw) const
{
    assert(Number() == handcardsBeforeDraw.Number() + 1);
    for (int i = 0; i < handcardsBeforeDraw.Number(); i++) {
        if (At(i) != handcardsBeforeDraw.At(i)) {
            return i;
        }
    }
    return handcardsBeforeDraw.Number();
}

bool Card::CanBePlayedAfter(Card lastPlayedCard, bool isUno)
{
    std::set<CardText> specialTexts{CardText::SKIP, CardText::REVERSE, 
        CardText::DRAW_TWO, CardText::WILD, CardText::DRAW_FOUR};

    // special cards can not be played as the last one
    if (isUno && specialTexts.count(mText)) {
        return false;
    }

    // if the last played card is skip, you can only play a skip
    if (lastPlayedCard.mText == CardText::SKIP) {
        return mText == CardText::SKIP;
    }

    // if the last played card is draw two, you can only play a draw two or draw four
    if (lastPlayedCard.mText == CardText::DRAW_TWO) {
        return (mText == CardText::DRAW_TWO || mText == CardText::DRAW_FOUR);
    }

    // if the last played card is draw four, you can only play a draw four
    if (lastPlayedCard.mText == CardText::DRAW_FOUR) {
        return mText == CardText::DRAW_FOUR;
    }

    // wild card can always be played except above conditions
    if (mColor == CardColor::BLACK) {
        return true;
    }

    // if not wild card, only cards with the same num or color can be played
    return (mColor == lastPlayedCard.mColor || mText == lastPlayedCard.mText);
}

void Deck::Init()
{
    this->Clear();
    mDiscardPile.Clear();
    for (auto color : CardSet::NonWildColors) {
        for (auto text : CardSet::NonWildTexts) {
            PushFront(color, text);
            if (text != CardText::ZERO) {
                // in UNO, there is only one zero for each color
                // and two cards for other text (except wild and wild draw four)
                PushFront(color, text);
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        // there are four `wild` and `wild draw four` each
        PushFront(CardColor::BLACK, CardText::WILD);
        PushFront(CardColor::BLACK, CardText::DRAW_FOUR);
    }

    Shuffle();
}

std::vector<std::array<Card, 7>> Deck::DealInitHandCards(int playerNum)
{
    std::vector<std::array<Card, 7>> initHandCards(playerNum);
    for (int card = 0; card < 7; card++) {
        for (int player = 0; player < playerNum; player++) {
            initHandCards[player][card] = Draw();
        }
    }
    return initHandCards;
}

Card Deck::Draw()
{
    if (Empty()) {
        Swap(mDiscardPile);
        Shuffle();
    }
    return PopFront();
}

std::vector<Card> Deck::Draw(int number)
{
    std::vector<Card> cards(number);
    std::generate(cards.begin(), cards.end(), [this]() { return Draw(); });
    return cards;
}

std::ostream& operator<<(std::ostream& os, const HandCards& handCards)
{
    os << "Your hand cards are: [" << handCards.ToString() << "]";
    return os;
}

Card::Card(const char *str)
{
    switch (*str) {
        case 'R': mColor = CardColor::RED;    str++; break;
        case 'Y': mColor = CardColor::YELLOW; str++; break;
        case 'G': mColor = CardColor::GREEN;  str++; break;
        case 'B': mColor = CardColor::BLUE;   str++; break;
        default:  mColor = CardColor::BLACK;
    }

    switch (*str) {
        case '0': mText = CardText::ZERO;    break;
        case '1': mText = CardText::ONE;     break;
        case '2': mText = CardText::TWO;     break;
        case '3': mText = CardText::THREE;   break;
        case '4': mText = CardText::FOUR;    break;
        case '5': mText = CardText::FIVE;    break;
        case '6': mText = CardText::SIX;     break;
        case '7': mText = CardText::SEVEN;   break;
        case '8': mText = CardText::EIGHT;   break;
        case '9': mText = CardText::NINE;    break;
        case 'S': mText = CardText::SKIP;    break;
        case 'R': mText = CardText::REVERSE; break;
        case 'W': mText = CardText::WILD;    break;
        case '+': mText = (*(str + 1) == '2') ? 
            CardText::DRAW_TWO : CardText::DRAW_FOUR; 
            break;
        case '\0': mText = CardText::EMPTY;  break;
        default: assert(0);
    }
}

std::string Card::ToString() const
{
    std::string color;
    std::string text;

    switch (mColor) {
        case CardColor::RED:    color = "R"; break;
        case CardColor::YELLOW: color = "Y"; break;
        case CardColor::GREEN:  color = "G"; break;
        case CardColor::BLUE:   color = "B"; break;
        case CardColor::BLACK:  color = "";  break;
        default: assert(0);
    }

    switch (mText) {
        case CardText::ZERO:        text = "0";  break;
        case CardText::ONE:         text = "1";  break;
        case CardText::TWO:         text = "2";  break;
        case CardText::THREE:       text = "3";  break;
        case CardText::FOUR:        text = "4";  break;
        case CardText::FIVE:        text = "5";  break;
        case CardText::SIX:         text = "6";  break;
        case CardText::SEVEN:       text = "7";  break;
        case CardText::EIGHT:       text = "8";  break;
        case CardText::NINE:        text = "9";  break;
        case CardText::SKIP:        text = "S";  break;
        case CardText::REVERSE:     text = "R";  break;
        case CardText::DRAW_TWO:    text = "+2"; break;
        case CardText::WILD:        text = "W";  break;
        case CardText::DRAW_FOUR:   text = "+4"; break;
        case CardText::EMPTY:       text = "";   break;
        default: assert(0);
    }

    return color.append(text);
}

int Card::Length() const
{
    int length = 0;
    length += (mColor == CardColor::BLACK ? 0 : 1);
    length += (!CardSet::DrawTexts.count(mText) ? 1 : 2);
    return length;
}

std::string HandCards::ToString() const
{
    return ToStringByCard(0, mCards.size());
}

std::string HandCards::ToStringBySegment(int seg) const
{
    int start = seg * 8;
    int len = std::min(static_cast<int>(mCards.size() - start), 8);
    return ToStringByCard(start, len);
}

std::string HandCards::ToStringByCard(int start, int len) const
{
    std::string str;
    std::for_each(std::next(mCards.begin(), start), std::next(mCards.begin(), start + len),
        [&str](Card card) {
            str.append(" ").append(card.ToString()).append(" ");
        }
    );
    return str;
}

int HandCards::Length() const
{
    return LengthBeforeIndex(mCards.size());
}

int HandCards::LengthBeforeIndex(int index) const
{
    int length = 0;
    std::for_each(mCards.begin(), IteratorAt(index), [&length](Card card) {
        // the length of card and spaces at both sides (e.g. " R4 ")
        length += (1 + card.Length() + 1);
    });
    return length;
}

int HandCards::LengthBeforeIndexInSegment(int segIndex, int indexInSeg) const
{
    int length = 0;
    int start = segIndex * 8;
    int len = indexInSeg;
    std::for_each(std::next(mCards.begin(), start), std::next(mCards.begin(), start + len),
        [&length](Card card) {
            length += (1 + card.Length() + 1);
        }
    );
    return length;
}

CardColor Card::FromChar(char c)
{
    switch (c) {
        case 'R': return CardColor::RED;
        case 'Y': return CardColor::YELLOW;
        case 'G': return CardColor::GREEN;
        case 'B': return CardColor::BLUE;
    }
    assert(0);
}

std::ostream& operator<<(std::ostream& os, const Card& card)
{
    os << card.ToString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const CardColor& color)
{
    std::string colorStr;
    switch (color) {
        case CardColor::RED:    colorStr = "RED";    break;
        case CardColor::YELLOW: colorStr = "YELLOW"; break;
        case CardColor::GREEN:  colorStr = "GREEN";  break;
        case CardColor::BLUE:   colorStr = "BLUE";   break;
        default: assert(0);
    }

    os << colorStr;
    return os;
}

}}