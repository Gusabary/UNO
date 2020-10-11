#include "game_board.h"

namespace UNO {

int Common::Common::mPlayerNum = 3;

namespace Game {

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
            if (Card::DrawTexts.count(flippedCard.mText)) {
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
            flippedCard, Common::Util::WrapWithPlayerNum(firstPlayer - player), tmpUsernames);

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
    }
}

void GameBoard::HandleDraw(const std::unique_ptr<DrawInfo> &info)
{
    std::cout << *info << std::endl;

    // draw from deck
    std::vector<Card> cardsToDraw = mDeck->Draw(info->mNumber);

    // respond to the deliverer
    mServer.DeliverInfo<DrawRspInfo>(mGameStat->GetCurrentPlayer(), info->mNumber, cardsToDraw);

    // broadcast to other players
    Broadcast<DrawInfo>(*info);

    // update stat
    mPlayerStats[mGameStat->GetCurrentPlayer()].UpdateAfterDraw(info->mNumber);
    mGameStat->UpdateAfterDraw();
}

void GameBoard::HandleSkip(const std::unique_ptr<SkipInfo> &info)
{
    std::cout << *info << std::endl;

    // broadcast to other players
    Broadcast<SkipInfo>(*info);

    // update stat
    mPlayerStats[mGameStat->GetCurrentPlayer()].UpdateAfterSkip();
    mGameStat->UpdateAfterSkip();
}

void GameBoard::HandlePlay(const std::unique_ptr<PlayInfo> &info)
{
    std::cout << *info << std::endl;
    
    mDiscardPile->Add(info->mCard);
    if (info->mCard.mColor == CardColor::BLACK) {
        // change the color to the specified next color to show in UI
        info->mCard.mColor = info->mNextColor;
    }

    // broadcast to other players
    Broadcast<PlayInfo>(*info);

    // update stat
    PlayerStat &stat = mPlayerStats[mGameStat->GetCurrentPlayer()];
    stat.UpdateAfterPlay(info->mCard);
    if (stat.GetRemainingHandCardsNum() == 0) {
        Win();
    }
    mGameStat->UpdateAfterPlay(info->mCard);
}

void GameBoard::Win()
{
    mGameStat->GameEnds();
    mServer.Close();
    std::cout << "Game Ends" << std::endl;
}

}}