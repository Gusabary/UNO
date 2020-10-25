#include "player.h"

namespace UNO { namespace Game {

Player::Player(std::string username, std::string host, std::string port)
    : mUsername(username), mClient(host, port), 
    mUIManager(std::make_unique<UIManager>(mGameStat, mPlayerStats, mHandCards))
{
    mClient.OnConnect = [this]() { JoinGame(); };

    mClient.Connect();
}

void Player::JoinGame()
{
    std::cout << "connect success, sending username to server" << std::endl;
    mClient.DeliverInfo<JoinGameInfo>(mUsername);

    // wait for game start
    std::unique_ptr<GameStartInfo> info = mClient.ReceiveInfo<GameStartInfo>();
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
            std::unique_ptr<ActionInfo> info = mClient.ReceiveInfo<ActionInfo>();
            switch (info->mActionType) {
                case ActionType::DRAW: {
                    auto drawInfo = std::unique_ptr<DrawInfo>(dynamic_cast<DrawInfo *>(info.release()));
                    std::cout << *drawInfo << std::endl;
                    UpdateStateAfterDraw(drawInfo->mPlayerIndex, drawInfo->mNumber);
                    break;
                }
                case ActionType::SKIP: {
                    auto skipInfo = std::unique_ptr<SkipInfo>(dynamic_cast<SkipInfo *>(info.release()));
                    std::cout << *skipInfo << std::endl;
                    UpdateStateAfterSkip(skipInfo->mPlayerIndex);
                    break;
                }
                case ActionType::PLAY: {
                    auto playInfo = std::unique_ptr<PlayInfo>(dynamic_cast<PlayInfo *>(info.release()));
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
    mClient.DeliverInfo<DrawInfo>(mGameStat->GetCardsNumToDraw());
    // wait for draw rsp msg
    std::unique_ptr<DrawRspInfo> info = mClient.ReceiveInfo<DrawRspInfo>();
    mHandCards->Draw(info->mCards);

    UpdateStateAfterDraw(0, mGameStat->GetCardsNumToDraw());
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
    mClient.DeliverInfo<SkipInfo>();
    UpdateStateAfterSkip(0);
}

bool Player::HandleSelfPlay(int cardIndex)
{
    Card cardToPlay = mHandCards->At(cardIndex);
    
    // if Play success, the card to play will be erased in handcards
    if (mHandCards->Play(cardIndex, mGameStat->GetLastPlayedCard())) {
        CardColor nextColor = (cardToPlay.mColor != CardColor::BLACK) ?
            cardToPlay.mColor : mUIManager->SpecifyNextColor();
        mClient.DeliverInfo<PlayInfo>(cardToPlay, nextColor);
        cardToPlay.mColor = nextColor;
        // the index of player himself is 0
        UpdateStateAfterPlay(0, cardToPlay);
        return true;
    }
    return false;
}

void Player::UpdateStateAfterDraw(int playerIndex, int number)
{
    mPlayerStats[playerIndex].UpdateAfterDraw(number);
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
