#include <ctime>

#include "game_board.h"

namespace UNO { namespace Game {

GameBoard::GameBoard(std::shared_ptr<Network::IServer> serverSp)
    : mServer(serverSp), 
    mDiscardPile(std::make_unique<DiscardPile>()),
    mDeck(std::make_unique<Deck>(*mDiscardPile))
{
    mServer->RegisterReceiveJoinGameInfoCallback(
        [this](int index, const JoinGameInfo &info) {
            ReceiveUsername(index, info.mUsername);
        }
    );
    mServer->RegisterAllPlayersJoinedCallback([this] { StartGame(); });
}

void GameBoard::Start()
{
    mServer->Run();
}

std::shared_ptr<Network::IServer> GameBoard::CreateServer(const std::string &port)
{
    return std::make_shared<Network::Server>(port);
}

void GameBoard::ResetGame()
{
    mServer->Reset();
    mPlayerStats.clear();
}

void GameBoard::ReceiveUsername(int index, const std::string &username)
{
    std::cout << "receive, index: " << index << ", username: " << username << std::endl;
    mPlayerStats.emplace_back(username, 7);
    std::vector<std::string> tmpUsernames;
    std::for_each(mPlayerStats.begin(), mPlayerStats.end(),
        [&tmpUsernames](const PlayerStat &stat) {
            tmpUsernames.push_back(stat.GetUsername());
        }
    );
    Common::Util::Deliver<JoinGameRspInfo>(mServer, index, Common::Common::mPlayerNum, tmpUsernames);
    for (int i = 0; i < index; i++) {
        Common::Util::Deliver<JoinGameInfo>(mServer, i, username);
    }
}

void GameBoard::StartGame()
{
#ifdef ENABLE_LOG
    spdlog::info("Game Starts.");
#endif
    mDeck->Init();
    std::vector<std::array<Card, 7>> initHandCards = 
        mDeck->DealInitHandCards(Common::Common::mPlayerNum);

    // flip a card
    Card flippedCard;
    while (true) {
        flippedCard = mDeck->Draw();
        if (flippedCard.mColor == CardColor::BLACK) {
            // if the flipped card is a wild card, put it to under the deck and flip a new one
            mDeck->PutToBottom(flippedCard);
        }
        else {
            if (CardSet::DrawTexts.count(flippedCard.mText)) {
                // last played card will become EMPTY if the flipped card is `Draw` card
                flippedCard.mText = CardText::EMPTY;
            }
            break;
        }
    }

    // choose the first player randomly
    std::srand(std::time(nullptr));
    int firstPlayer = std::rand() % Common::Common::mPlayerNum;

    std::vector<std::string> tmpUsernames;
    std::for_each(mPlayerStats.begin(), mPlayerStats.end(),
        [&tmpUsernames](const PlayerStat &stat) {
            tmpUsernames.push_back(stat.GetUsername());
        }
    );
    for (int player = 0; player < Common::Common::mPlayerNum; player++) {
        Common::Util::Deliver<GameStartInfo>(mServer, player, initHandCards[player], flippedCard,
            Common::Util::WrapWithPlayerNum(firstPlayer - player), tmpUsernames);

        std::rotate(tmpUsernames.begin(), tmpUsernames.begin() + 1, tmpUsernames.end());
    }

    mGameStat.reset(new GameStat(firstPlayer, flippedCard));
    GameLoop();
}

void GameBoard::GameLoop()
{
    while (!mGameStat->DoesGameEnd()) {
        try {
            auto actionInfo = Common::Util::Receive<ActionInfo>(mServer, mGameStat->GetCurrentPlayer());
            switch (actionInfo->mActionType) {
                case ActionType::DRAW:
                    HandleDraw(Common::Util::DynamicCast<DrawInfo>(actionInfo));
                    break;
                case ActionType::SKIP:
                    HandleSkip(Common::Util::DynamicCast<SkipInfo>(actionInfo));
                    break;
                case ActionType::PLAY:
                    HandlePlay(Common::Util::DynamicCast<PlayInfo>(actionInfo));
                    break;
                default:
                    assert(0);
            }
        }
        catch (const std::exception &e) {
            /// TODO: handle the condition that someone has disconnected
            std::cout << "someone has disconnected, shutdown server" << std::endl;
            std::exit(-1);
        }
    }
    ResetGame();
}

void GameBoard::HandleDraw(const std::unique_ptr<DrawInfo> &info)
{
    std::cout << *info << std::endl;

    // draw from deck
    std::vector<Card> cardsToDraw = mDeck->Draw(info->mNumber);

    // respond to the deliverer
    Common::Util::Deliver<DrawRspInfo>(mServer, mGameStat->GetCurrentPlayer(), 
        info->mNumber, cardsToDraw);

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
#ifdef ENABLE_LOG
    spdlog::info("Game Ends.");
#endif
}

}}