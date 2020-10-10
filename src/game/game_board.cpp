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
            if (Card::DrawTexts.count(flippedCard.mText) > 0) {
                // last played card will become EMPTY if the flipped card is `Draw` card
                flippedCard.mText = CardText::EMPTY;
            }
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
        mServer.DeliverInfo<GameStartInfo>(player, initHandCards[player], 
            flippedCard, Util::WrapWithPlayerNum(firstPlayer - player, PLAYER_NUM), tmpUsernames);

        std::rotate(tmpUsernames.begin(), tmpUsernames.begin() + 1, tmpUsernames.end());
    }

    mGameStat.reset(new GameStat(firstPlayer, flippedCard));
    GameLoop();
}

void GameBoard::GameLoop()
{
    while (!mGameStat->DoesGameEnd()) {
        std::unique_ptr<ActionInfo> info = mServer.ReceiveInfo<ActionInfo>(mGameStat->GetCurrentPlayer());
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
        mGameStat->NextPlayer(PLAYER_NUM);
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

    int currentPlayer = mGameStat->GetCurrentPlayer();
    // update player stats
    mPlayerStats[currentPlayer].UpdateAfterDraw(info->mNumber);
    
    // respond to the deliverer
    mServer.DeliverInfo<DrawRspInfo>(currentPlayer, info->mNumber, cardsToDraw);

    // broadcast to other players
    for (int i = 0; i < PLAYER_NUM; i++) {
        if (i != currentPlayer) {
            info->mPlayerIndex = Util::WrapWithPlayerNum(currentPlayer - i, PLAYER_NUM);
            mServer.DeliverInfo<DrawInfo>(i, *info);
        }
    }
}

void GameBoard::HandleSkip(std::unique_ptr<SkipInfo> info)
{
    std::cout << *info << std::endl;

    int currentPlayer = mGameStat->GetCurrentPlayer();
    // update player stats
    mPlayerStats[currentPlayer].UpdateAfterSkip();

    // no response to the deliverer

    // broadcast to other players
    for (int i = 0; i < PLAYER_NUM; i++) {
        if (i != currentPlayer) {
            info->mPlayerIndex = Util::WrapWithPlayerNum(currentPlayer - i, PLAYER_NUM);
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
        mGameStat->Reverse();
    }
    if (info->mCard.mColor == CardColor::BLACK) {
        // change the color to the specified next color to show in UI
        info->mCard.mColor = info->mNextColor;
    }

    int currentPlayer = mGameStat->GetCurrentPlayer();
    // update player stats
    PlayerStat &stat = mPlayerStats[currentPlayer];
    stat.UpdateAfterPlay(info->mCard);
    if (stat.GetRemainingHandCardsNum() == 0) {
        Win();
    }

    // no response to the deliverer

    // broadcast to other players
    for (int i = 0; i < PLAYER_NUM; i++) {
        if (i != currentPlayer) {
            info->mPlayerIndex = Util::WrapWithPlayerNum(currentPlayer - i, PLAYER_NUM);
            mServer.DeliverInfo<PlayInfo>(i, *info);
        }
    }
}

void GameBoard::Win()
{
    mGameStat->GameEnds();
    mServer.Close();
    std::cout << "Game Ends" << std::endl;
}

}}