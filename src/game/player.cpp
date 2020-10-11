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

    GameLoop();
}

void Player::GameLoop()
{
    while (!mGameStat->DoesGameEnd()) {
        mUIManager->Render();
        if (mGameStat->IsMyTurn()) {
            while (true) {
                std::string input = mUIManager->GetAction();
                if (input == "D") {
                    HandleSelfDraw();
                    break;
                }
                else if (input == "S") {
                    HandleSelfSkip();
                    break;
                }
                else if (input[0] == 'P') {
                    if (HandleSelfPlay(std::stoi(input.substr(1)))) {
                        break;
                    }
                }
            }
        }
        else {
            // wait for gameboard state update from server
            std::unique_ptr<ActionInfo> info = mClient.ReceiveInfo<ActionInfo>();
            switch (info->mActionType) {
                case ActionType::DRAW:
                    HandleOtherDraw(std::unique_ptr<DrawInfo>(dynamic_cast<DrawInfo *>(info.release())));
                    break;
                case ActionType::SKIP:
                    HandleOtherSkip(std::unique_ptr<SkipInfo>(dynamic_cast<SkipInfo *>(info.release())));
                    break;
                case ActionType::PLAY:
                    HandleOtherPlay(std::unique_ptr<PlayInfo>(dynamic_cast<PlayInfo *>(info.release())));
                    break;
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
}

void Player::HandleSelfSkip()
{
    mClient.DeliverInfo<SkipInfo>();
    UpdateStateAfterSkip(0);
}

bool Player::HandleSelfPlay(int cardIndex)
{
    char nextColor = ' ';
    Card cardToPlay = mHandCards->At(cardIndex);
    if (cardToPlay.mColor == CardColor::BLACK) {
        while (!std::set<char>{'R', 'Y', 'G', 'B'}.count(nextColor)) {
            nextColor = mUIManager->SpecifyNextColor();
        }
    }
    if (mHandCards->Play(cardIndex, mGameStat->GetLastPlayedCard())) {
        if (nextColor == ' ') {
            mClient.DeliverInfo<PlayInfo>(cardToPlay);
        }
        else {
            mClient.DeliverInfo<PlayInfo>(cardToPlay, Card::FromChar(nextColor));
        }
        // the index of player himself is 0
        UpdateStateAfterPlay(0, cardToPlay);
        return true;
    }
    return false;
}

void Player::HandleOtherDraw(const std::unique_ptr<DrawInfo> &info)
{
    std::cout << *info << std::endl;
    UpdateStateAfterDraw(info->mPlayerIndex, info->mNumber);
}

void Player::HandleOtherSkip(const std::unique_ptr<SkipInfo> &info)
{
    std::cout << *info << std::endl;
    UpdateStateAfterSkip(info->mPlayerIndex);
}

void Player::HandleOtherPlay(const std::unique_ptr<PlayInfo> &info)
{
    std::cout << *info << std::endl;
    UpdateStateAfterPlay(info->mPlayerIndex, info->mCard);
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
