#include "player.h"

namespace UNO { namespace Game {

Player::Player(std::string username, std::shared_ptr<Network::IClient> clientSp)
    : mUsername(username), mClient(clientSp)
{
    mClient->RegisterConnectCallback([this] { JoinGame(); });

    mClient->Connect();
}

std::shared_ptr<Network::IClient> Player::CreateClient(const std::string &host, const std::string &port)
{
    return std::make_shared<Network::Client>(host, port);
}

void Player::ResetGame()
{
    mClient->Reset();
    mPlayerStats.clear();
}

void Player::JoinGame()
{
    // std::cout << "connect success, sending username to server" << std::endl;
    Common::Util::Deliver<JoinGameInfo>(mClient, mUsername);

    auto joinRsp = Common::Util::Receive<JoinGameRspInfo>(mClient);
    // std::cout << *joinRsp << std::endl;
    auto initUsernames = joinRsp->mUsernames;
    auto initSize = initUsernames.size();
    // don't forget to update common config
    Common::Common::mPlayerNum = joinRsp->mPlayerNum;
    // UIManager should be initialized after common config being loaded
    mUIManager = std::make_unique<UIManager>(mGameStat, mPlayerStats, mHandCards);
    mUIManager->RenderWhenInitWaiting(initUsernames, true);
    for (auto i = 0; i < Common::Common::mPlayerNum - initSize; i++) {
        auto joinInfo = Common::Util::Receive<JoinGameInfo>(mClient);
        initUsernames.push_back(joinInfo->mUsername);
        mUIManager->RenderWhenInitWaiting(initUsernames, false);
    }

    // wait for game start
    auto info = Common::Util::Receive<GameStartInfo>(mClient);
    // std::cout << *info << std::endl;

    mHandCards.reset(new HandCards(info->mInitHandCards));
    mGameStat.reset(new GameStat(*info));
    std::for_each(info->mUsernames.begin(), info->mUsernames.end(), 
        [this](const std::string &username) {
            mPlayerStats.emplace_back(username, 7);
        }
    );

    mUIManager->RunTimerThread();
    GameLoop();
}

void Player::GameLoop()
{
    while (!mGameStat->DoesGameEnd()) {
        if (mGameStat->IsMyTurn()) {
            // when it's my turn, reset the cursor for a better ui,
            // except the condition that having a chance to play immediately after draw
            if (!mPlayerStats[0].HasChanceToPlayAfterDraw()) {
                mUIManager->NextTurn();
            }
            bool actionSuccess = false;
            bool lastCardCanBePlayed = true;
            while (!actionSuccess) {
                auto [action, cardIndex] = mUIManager->GetAction(lastCardCanBePlayed,
                        mPlayerStats[0].HasChanceToPlayAfterDraw());
                switch (action) {
                    case InputAction::PASS: {
                        if (mPlayerStats[0].HasChanceToPlayAfterDraw() || mGameStat->IsSkipped()) {
                            HandleSelfSkip();
                        }
                        else {
                            HandleSelfDraw();
                        }
                        actionSuccess = true;
                        break;
                    }
                    case InputAction::PLAY: {
                        actionSuccess = HandleSelfPlay(cardIndex);
                        // if action succeeded, ok, nothing happens.
                        // while if failure, lastCardCanBePlayed will be set to false, 
                        // which will affect the hint text.
                        lastCardCanBePlayed = actionSuccess;
                        break;
                    }
                    default:
                        assert(0);
                }
            }
        }
        else {
            if (!mPlayerStats[mGameStat->GetCurrentPlayer()].HasChanceToPlayAfterDraw()) {
                mUIManager->NextTurn();
            }
            mUIManager->Render();
            // wait for gameboard state update from server
            auto info = Common::Util::Receive<ActionInfo>(mClient);
            switch (info->mActionType) {
                case ActionType::DRAW: {
                    auto drawInfo = Common::Util::DynamicCast<DrawInfo>(info);
                    // std::cout << *drawInfo << std::endl;
                    UpdateStateAfterDraw(drawInfo->mPlayerIndex, drawInfo->mNumber);
                    break;
                }
                case ActionType::SKIP: {
                    auto skipInfo = Common::Util::DynamicCast<SkipInfo>(info);
                    // std::cout << *skipInfo << std::endl;
                    UpdateStateAfterSkip(skipInfo->mPlayerIndex);
                    break;
                }
                case ActionType::PLAY: {
                    auto playInfo = Common::Util::DynamicCast<PlayInfo>(info);
                    // std::cout << *playInfo << std::endl;
                    UpdateStateAfterPlay(playInfo->mPlayerIndex, playInfo->mCard);
                    break;
                }
                default:
                    assert(0);
            }
        }
    }
    // show one more frame after win
    mUIManager->Render();
    GameEnds();
}

void Player::GameEnds()
{
    // let the server resets game first
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (!mUIManager->WantToPlayAgain(mWinner)) {
        std::exit(0);
    }
    ResetGame();
}

void Player::HandleSelfDraw()
{
    Common::Util::Deliver<DrawInfo>(mClient, mGameStat->GetCardsNumToDraw());
    // wait for draw rsp msg
    auto info = Common::Util::Receive<DrawRspInfo>(mClient);
    auto handcardsBeforeDraw = *mHandCards;
    int indexOfNewlyDrawn = -1;
    mHandCards->Draw(info->mCards);
    if (info->mNumber == 1) {
        indexOfNewlyDrawn = mHandCards->GetIndexOfNewlyDrawn(handcardsBeforeDraw);
    }

    UpdateStateAfterDraw(0, mGameStat->GetCardsNumToDraw(), indexOfNewlyDrawn);
    if (!mPlayerStats[0].HasChanceToPlayAfterDraw()) {
        // draw penalty due to a +2 / +4, cannot play immediately
        HandleSelfSkip();
    }
    else {
        // a common draw, move the cursor to the card just drawn
        assert(info->mCards.size() == 1);
        Card cardDrawn = info->mCards.front();
        int cursorIndex = mHandCards->GetIndex(cardDrawn);
        mUIManager->MoveCursorTo(cursorIndex);
    }
}

void Player::HandleSelfSkip()
{
    Common::Util::Deliver<SkipInfo>(mClient);
    UpdateStateAfterSkip(0);
}

bool Player::HandleSelfPlay(int cardIndex)
{
    Card cardToPlay = mHandCards->At(cardIndex);
    
    if (mHandCards->CanBePlayedAfter(cardIndex, mGameStat->GetLastPlayedCard())) {
        // the card to play should be erased **after** specifying next color if it's wild card
        CardColor nextColor = (cardToPlay.mColor != CardColor::BLACK) ?
            cardToPlay.mColor : mUIManager->SpecifyNextColor();
        Common::Util::Deliver<PlayInfo>(mClient, cardToPlay, nextColor);
        cardToPlay.mColor = nextColor;
        UpdateStateAfterPlay(0, cardToPlay);
        mHandCards->Erase(cardIndex);
        return true;
    }
    return false;
}

void Player::UpdateStateAfterDraw(int playerIndex, int number, int indexOfNewlyDrawn)
{
    mPlayerStats[playerIndex].UpdateAfterDraw(number, indexOfNewlyDrawn);
    mGameStat->UpdateAfterDraw();
}

void Player::UpdateStateAfterSkip(int playerIndex)
{
    mPlayerStats[playerIndex].UpdateAfterSkip();
    mGameStat->UpdateAfterSkip();
}

void Player::UpdateStateAfterPlay(int playerIndex, Card cardPlayed)
{
    PlayerStat &stat = mPlayerStats[playerIndex];
    stat.UpdateAfterPlay(cardPlayed);
    if (stat.GetRemainingHandCardsNum() == 0) {
        Win(playerIndex);
    }

    mGameStat->UpdateAfterPlay(cardPlayed);
}

void Player::Win(int playerIndex)
{
    mUIManager->StopTimerThread();
    mGameStat->GameEnds();
    mWinner = (playerIndex == 0) ? "You" : mPlayerStats[playerIndex].GetUsername();
}

void Player::PrintLocalState()
{
    std::cout << "Local State: " << std::endl;
    std::cout << "\t " << *mHandCards << std::endl;
    std::cout << "\t mLastPlayedCard: " << mGameStat->GetLastPlayedCard() << std::endl;
    std::cout << "\t mCurrentPlayer: " << mGameStat->GetCurrentPlayer() << std::endl;
    std::cout << "\t mIsInClockwise: " << mGameStat->IsInClockwise() << std::endl;
    std::cout << "\t mCardsNumToDraw: " << mGameStat->GetCardsNumToDraw() << std::endl;

    std::cout << "\t mPlayerStats: [" << std::endl;
    for (const auto &stat : mPlayerStats) {
        std::cout << "  " << stat << std::endl;
    }
    std::cout << "\t ]" << std::endl;
}

}}
