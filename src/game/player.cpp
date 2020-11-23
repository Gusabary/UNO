#include "player.h"

namespace UNO { namespace Game {

Player::Player(std::string username, std::unique_ptr<Network::Client> &clientUp)
    : mUsername(username), mClient(clientUp.release()), 
    mUIManager(std::make_unique<UIManager>(mGameStat, mPlayerStats, mHandCards))
{
    mClient->RegisterConnectCallback([this] { JoinGame(); });

    mClient->Connect();
}

std::unique_ptr<Network::Client> Player::CreateClient(const std::string &host, const std::string &port)
{
    return std::make_unique<Network::Client>(host, port);
}

void Player::JoinGame()
{
    std::cout << "connect success, sending username to server" << std::endl;
    mClient->DeliverInfo(typeid(JoinGameInfo), JoinGameInfo{mUsername});

    auto joinRsp = Common::Util::DynamicCast<JoinGameRspInfo>(mClient->ReceiveInfo(typeid(JoinGameRspInfo)));
    auto initUsernames = joinRsp->mUsernames;
    auto initSize = initUsernames.size();
    // don't forget to update common config
    Common::Common::mPlayerNum = joinRsp->mPlayerNum;
    mUIManager->RenderWhenInitWaiting(initUsernames);
    for (auto i = 0; i < Common::Common::mPlayerNum - initSize; i++) {
        auto joinInfo = Common::Util::DynamicCast<JoinGameInfo>(mClient->ReceiveInfo(typeid(JoinGameInfo)));
        initUsernames.push_back(joinInfo->mUsername);
        mUIManager->RenderWhenInitWaiting(initUsernames);
    }

    // wait for game start
    auto info = Common::Util::DynamicCast<GameStartInfo>(mClient->ReceiveInfo(typeid(GameStartInfo)));
    std::cout << *info << std::endl;

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
            auto info = Common::Util::DynamicCast<ActionInfo>(mClient->ReceiveInfo(typeid(ActionInfo)));
            switch (info->mActionType) {
                case ActionType::DRAW: {
                    auto drawInfo = Common::Util::DynamicCast<DrawInfo>(info);
                    std::cout << *drawInfo << std::endl;
                    UpdateStateAfterDraw(drawInfo->mPlayerIndex, drawInfo->mNumber);
                    break;
                }
                case ActionType::SKIP: {
                    auto skipInfo = Common::Util::DynamicCast<SkipInfo>(info);
                    std::cout << *skipInfo << std::endl;
                    UpdateStateAfterSkip(skipInfo->mPlayerIndex);
                    break;
                }
                case ActionType::PLAY: {
                    auto playInfo = Common::Util::DynamicCast<PlayInfo>(info);
                    std::cout << *playInfo << std::endl;
                    UpdateStateAfterPlay(playInfo->mPlayerIndex, playInfo->mCard);
                    break;
                }
                default:
                    assert(0);
            }
        }
    }
}

void Player::HandleSelfDraw()
{
    mClient->DeliverInfo(typeid(DrawInfo), DrawInfo{mGameStat->GetCardsNumToDraw()});
    // wait for draw rsp msg
    auto info = Common::Util::DynamicCast<DrawRspInfo>(mClient->ReceiveInfo(typeid(DrawRspInfo)));
    auto handcardsBeforeDraw = *mHandCards;
    int indexOfNewlyDrawn;
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
    mClient->DeliverInfo(typeid(SkipInfo), SkipInfo{});
    UpdateStateAfterSkip(0);
}

bool Player::HandleSelfPlay(int cardIndex)
{
    Card cardToPlay = mHandCards->At(cardIndex);
    
    if (mHandCards->CanBePlayedAfter(cardIndex, mGameStat->GetLastPlayedCard())) {
        // the card to play should be erased **after** specifying next color if it's wild card
        CardColor nextColor = (cardToPlay.mColor != CardColor::BLACK) ?
            cardToPlay.mColor : mUIManager->SpecifyNextColor();
        mClient->DeliverInfo(typeid(PlayInfo), PlayInfo{cardToPlay, nextColor});
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
    mGameStat->GameEnds();
    if (playerIndex == 0) {
        std::cout << "You win!" << std::endl;
    }
    else {
        std::string winner = mPlayerStats[playerIndex].GetUsername();
        std::cout << winner << " wins!" << std::endl;
    }
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
