#include "game_board.h"

namespace UNO { namespace Game {

GameBoard::GameBoard(std::string port)
    : mServer(port)
{
    mServer.OnReceiveJoinGameInfo = [this](int index, const JoinGameInfo &info) {
        ReceiveUsername(index, info.mUsername);
    };

    mServer.Run();
}

void GameBoard::ReceiveUsername(int index, const std::string &username)
{
    std::cout << "receive, index: " << index << ", username: " << username << std::endl;
    mPlayerStats.emplace_back(username, 7);
    if (index == PLAYER_NUM - 1) {
        StartGame();
    }
}

void GameBoard::StartGame()
{
    InitDeck();
    std::vector<std::array<Card, 7>> initHandCards = DealInitHandCards();

    // flip a card
    Card flippedCard;
    while (true) {
        flippedCard = mDeck.front();
        mDeck.pop_front();
        if (flippedCard.mColor == CardColor::BLACK) {
            // if the flipped card is a wild card, put it to under the deck and flip a new one
            mDeck.push_back(flippedCard);
        }
        else {
            break;
        }
    }

    // choose the first player randomly
    std::srand(std::time(nullptr));
    int firstPlayer = std::rand() % PLAYER_NUM;

    std::vector<std::string> tmpUsernames;
    std::for_each(mPlayerStats.begin(), mPlayerStats.end(),
        [&tmpUsernames](const PlayerStat &stat) {
            tmpUsernames.push_back(stat.GetUsername());
        }
    );
    for (int player = 0; player < PLAYER_NUM; player++) {
        mServer.DeliverInfo<GameStartInfo>(player, 
            initHandCards[player], flippedCard, WrapWithPlayerNum(firstPlayer - player), tmpUsernames);

        std::rotate(tmpUsernames.begin(), tmpUsernames.begin() + 1, tmpUsernames.end());
    }

    mCurrentPlayer = firstPlayer;
    mIsInClockwise = (flippedCard.mText == CardText::REVERSE) ? false : true;
    GameLoop();
}

void GameBoard::GameLoop()
{
    while (!mGameEnds) {
        std::unique_ptr<ActionInfo> info = mServer.ReceiveInfo<ActionInfo>(mCurrentPlayer);
        switch (info->mActionType) {
            case ActionType::DRAW:
                HandleDraw(std::unique_ptr<DrawInfo>(dynamic_cast<DrawInfo *>(info.release())));
                break;
            case ActionType::SKIP:
                HandleSkip(std::unique_ptr<SkipInfo>(dynamic_cast<SkipInfo *>(info.release())));
                break;
            case ActionType::PLAY:
                HandlePlay(std::unique_ptr<PlayInfo>(dynamic_cast<PlayInfo *>(info.release())));
                break;
            default:
                assert(0);
        }

        // update mCurrentPlayer
        mCurrentPlayer = mIsInClockwise ? WrapWithPlayerNum(mCurrentPlayer + 1) : WrapWithPlayerNum(mCurrentPlayer - 1);
    }
}

void GameBoard::HandleDraw(std::unique_ptr<DrawInfo> info)
{
    std::cout << *info << std::endl;

    // draw from deck
    std::vector<Card> cardsToDraw(info->mNumber);
    for (auto &cardToDraw : cardsToDraw) {
        cardToDraw = mDeck.front();
        mDeck.pop_front();
    }

    // update player stats
    mPlayerStats[mCurrentPlayer].UpdateAfterDraw(info->mNumber);
    
    // respond to the deliverer
    mServer.DeliverInfo<DrawRspInfo>(mCurrentPlayer, info->mNumber, cardsToDraw);

    // broadcast to other players
    for (int i = 0; i < PLAYER_NUM; i++) {
        if (i != mCurrentPlayer) {
            info->mPlayerIndex = WrapWithPlayerNum(mCurrentPlayer - i);
            mServer.DeliverInfo<DrawInfo>(i, *info);
        }
    }
}

void GameBoard::HandleSkip(std::unique_ptr<SkipInfo> info)
{
    std::cout << *info << std::endl;

    // update player stats
    mPlayerStats[mCurrentPlayer].UpdateAfterSkip();

    // no response to the deliverer

    // broadcast to other players
    for (int i = 0; i < PLAYER_NUM; i++) {
        if (i != mCurrentPlayer) {
            info->mPlayerIndex = WrapWithPlayerNum(mCurrentPlayer - i);
            mServer.DeliverInfo<SkipInfo>(i, *info);
        }
    }
}

void GameBoard::HandlePlay(std::unique_ptr<PlayInfo> info)
{
    std::cout << *info << std::endl;
    
    // update local state:
    // add the card just played into discard pile
    // and reverse mIsInClockwise if necessary
    mDiscardPile.push_front(info->mCard);
    if (info->mCard.mText == CardText::REVERSE) {
        mIsInClockwise = !mIsInClockwise;
    }

    // update player stats
    PlayerStat &stat = mPlayerStats[mCurrentPlayer];
    stat.UpdateAfterPlay(info->mCard);
    if (stat.GetRemainingHandCardsNum() == 0) {
        Win();
    }

    // no response to the deliverer

    // broadcast to other players
    for (int i = 0; i < PLAYER_NUM; i++) {
        if (i != mCurrentPlayer) {
            info->mPlayerIndex = WrapWithPlayerNum(mCurrentPlayer - i);
            mServer.DeliverInfo<PlayInfo>(i, *info);
        }
    }
}

void GameBoard::Win()
{
    mGameEnds = true;
    mServer.Close();
    std::cout << "Game Ends" << std::endl;
}

void GameBoard::InitDeck()
{
    std::initializer_list<CardColor> colors = { CardColor::RED, CardColor::YELLOW, 
        CardColor::GREEN, CardColor::BLUE };
    
    std::initializer_list<CardText> texts = { CardText::ZERO, CardText::ONE, CardText::TWO, 
        CardText::THREE, CardText::FOUR, CardText::FIVE, CardText::SIX, CardText::SEVEN, 
        CardText::EIGHT, CardText::NINE, CardText::SKIP, CardText::REVERSE, CardText::DRAW_TWO};
    
    mDeck.clear();
    for (auto color : colors) {
        for (auto text : texts) {
            mDeck.emplace_back(color, text);
            if (text != CardText::ZERO) {
                // in UNO, there is only one zero for each color
                // and two cards for other text (except wild and wild draw four)
                mDeck.emplace_back(color, text);
            }
        }
    }

   for (int i = 0; i < 4; i++) {
        // there are four `wild` and `wild draw four` each
        mDeck.emplace_back(CardColor::BLACK, CardText::WILD);
        mDeck.emplace_back(CardColor::BLACK, CardText::DRAW_FOUR);
    }

    // shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(mDeck.begin(), mDeck.end(), g);

    mDiscardPile.clear();
}

std::vector<std::array<Card, 7>> GameBoard::DealInitHandCards()
{
    std::vector<std::array<Card, 7>> initHandCards(PLAYER_NUM);
    for (int card = 0; card < 7; card++) {
        for (int player = 0; player < PLAYER_NUM; player++) {
            initHandCards[player][card] = mDeck.front();
            mDeck.pop_front();
        }
    }
    return initHandCards;
}

int GameBoard::WrapWithPlayerNum(int numToWrap)
{
    int ret = numToWrap % PLAYER_NUM;
    if (ret < 0) {
        ret += PLAYER_NUM;
    }
    return ret;
}
}}