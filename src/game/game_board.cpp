#include "game_board.h"

namespace UNO { namespace Game {

GameBoard::GameBoard(std::string port)
    : mServer(port), mDiscardPile(std::make_unique<DiscardPile>()),
    mDeck(std::make_unique<Deck>(*mDiscardPile))
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
    mDeck->Init();
    std::vector<std::array<Card, 7>> initHandCards = mDeck->DealInitHandCards(PLAYER_NUM);

    // flip a card
    Card flippedCard;
    while (true) {
        flippedCard = mDeck->Draw();
        if (flippedCard.mColor == CardColor::BLACK) {
            // if the flipped card is a wild card, put it to under the deck and flip a new one
            mDeck->PutToBottom(flippedCard);
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
        cardToDraw = mDeck->Draw();
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
    mDiscardPile->Add(info->mCard);
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

int GameBoard::WrapWithPlayerNum(int numToWrap)
{
    int ret = numToWrap % PLAYER_NUM;
    if (ret < 0) {
        ret += PLAYER_NUM;
    }
    return ret;
}
}}