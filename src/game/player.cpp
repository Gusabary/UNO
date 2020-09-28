#include "player.h"

namespace UNO { namespace Game {

Player::Player(std::string username, std::string host, std::string port)
    : mUsername(username), mClient(host, port) 
{
    mClient.OnConnect = [this]() { JoinGame(); };

    mClient.Connect();
}

void Player::JoinGame()
{
    std::cout << "connect success, sending username to server" << std::endl;
    mClient.DeliverInfo<JoinGameInfo>(JoinGameInfo(mUsername));

    // wait for game start
    std::unique_ptr<GameStartInfo> info = mClient.ReceiveInfo<GameStartInfo>();
    std::cout << *info << std::endl;

    mHandCards.resize(info->mInitHandCards.size());
    std::copy(info->mInitHandCards.begin(), info->mInitHandCards.end(), mHandCards.begin());
    mLastPlayedCard = info->mFlippedCard;
    mCurrentPlayer = info->mFirstPlayer;
    std::for_each(info->mUsernames.begin(), info->mUsernames.end(), 
        [this](const std::string &username) {
            mPlayerStats.emplace_back(username, 7);
        }
    );
    // TODO: if the flippedCard is reverse
    mIsInClockwise = true;
    mCardsNumToDraw = 1;
    GameLoop();
}

void Player::GameLoop()
{
    while (true) {
        if (mCurrentPlayer == 0) {
            std::string input;
            std::cout << "Now it's your turn" << std::endl;
            std::cin >> input;
            if (input == "D") {
                // Draw
                mClient.DeliverInfo<DrawInfo>(DrawInfo(mCardsNumToDraw));

                // wait for draw rsp msg
                std::unique_ptr<DrawRspInfo> info = mClient.ReceiveInfo<DrawRspInfo>();
                std::cout << *info << std::endl;

                std::for_each(info->mCards.begin(), info->mCards.end(),
                    [this](const Card &card) {
                        mHandCards.push_back(card);
                    }
                );
            }
            else if (input == "S") {
                // Skip
                mClient.DeliverInfo<SkipInfo>(SkipInfo());
            }
            else if (input == "P") {
                // Play
                mClient.DeliverInfo<PlayInfo>(PlayInfo(Card(CardColor::RED, CardText::ZERO), CardColor::YELLOW));
            }
        }
        else {
            // wait for gameboard state update from server
            std::unique_ptr<ActionInfo> info = mClient.ReceiveInfo<ActionInfo>();
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

        // update local state
        mCurrentPlayer = (mCurrentPlayer + 1) % mPlayerStats.size();
    }
}

void Player::HandleDraw(const std::unique_ptr<DrawInfo> &info)
{
    std::cout << *info << std::endl;
    PlayerStat &stat = mPlayerStats[info->mPlayerIndex];
    stat.mRemainingHandCardsNum += info->mNumber;
    stat.mDoPlayInLastRound = false;
}

void Player::HandleSkip(const std::unique_ptr<SkipInfo> &info)
{
    std::cout << *info << std::endl;
    PlayerStat &stat = mPlayerStats[info->mPlayerIndex];
    stat.mDoPlayInLastRound = false;
}

void Player::HandlePlay(const std::unique_ptr<PlayInfo> &info)
{
    std::cout << *info << std::endl;
    PlayerStat &stat = mPlayerStats[info->mPlayerIndex];
    stat.mRemainingHandCardsNum -= 1;
    stat.mDoPlayInLastRound = true;
    stat.mLastPlayedCard = info->mCard;
    mLastPlayedCard = info->mCard;
}
}}
