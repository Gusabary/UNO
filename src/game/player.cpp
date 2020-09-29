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

    mIsInClockwise = (info->mFlippedCard.mText == CardText::REVERSE) ? false : true;
    mCardsNumToDraw = 1;
    GameLoop();
}

void Player::GameLoop()
{
    while (true) {
        if (mCurrentPlayer == 0) {
            char inputBuffer[10];
            std::cout << "Now it's your turn, your hand cards are: [";
            for (int i = 0; i < mHandCards.size() - 1; i++) {
                std::cout << mHandCards[i] << ", ";
            }
            std::cout << mHandCards.back() << "]" << std::endl;
            while (true) {
                std::cout << "Input (D)raw, (S)kip or (P)lay <card_index>:" << std::endl;
                std::cin.getline(inputBuffer, 10);
                std::string input(inputBuffer);
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
                    break;
                }
                else if (input == "S") {
                    // Skip
                    mClient.DeliverInfo<SkipInfo>(SkipInfo());
                    break;
                }
                else if (input[0] == 'P') {
                    // Play
                    int cardIndex = std::stoi(input.substr(1));
                    if (cardIndex < mHandCards.size()) {
                        Card cardToPlay = mHandCards[cardIndex];
                        if (CanCardBePlayed(cardToPlay)) {
                            mHandCards.erase(mHandCards.begin() + cardIndex);
                            mClient.DeliverInfo<PlayInfo>(PlayInfo(cardToPlay));
                            UpdateStateAfterPlaying(cardToPlay);
                            break;
                        }
                    }
                }
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

        // update mCurrentPlayer
        mCurrentPlayer = mIsInClockwise ? WrapWithPlayerNum(mCurrentPlayer + 1) : WrapWithPlayerNum(mCurrentPlayer - 1);
        PrintLocalState();
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
    UpdateStateAfterPlaying(info->mCard);
}

void Player::UpdateStateAfterPlaying(Card cardPlayed)
{
    mLastPlayedCard = cardPlayed;
    if (cardPlayed.mText == CardText::REVERSE) {
        mIsInClockwise = !mIsInClockwise;
    }
    if (cardPlayed.mText == CardText::DRAW_TWO) {
        // in the normal state, mCardsNumToDraw is equal to 1
        // once a player plays a `Draw` card, the effect is gonna accumulate
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 2 : (mCardsNumToDraw + 2);
    }
    if (cardPlayed.mText == CardText::DRAW_FOUR) {
        mCardsNumToDraw = (mCardsNumToDraw == 1) ? 4 : (mCardsNumToDraw + 4);
    }

    mPlayerStats[mCurrentPlayer].mRemainingHandCardsNum--;
    mPlayerStats[mCurrentPlayer].mDoPlayInLastRound = true;
    mPlayerStats[mCurrentPlayer].mLastPlayedCard = cardPlayed;
}

bool Player::CanCardBePlayed(Card cardToPlay)
{
    return true;
}

int Player::WrapWithPlayerNum(int numToWrap)
{
    int playerNum = mPlayerStats.size();
    int ret = numToWrap % playerNum;
    if (ret < 0) {
        ret += playerNum;
    }
    return ret;
}

void Player::PrintLocalState()
{
    std::cout << "Local State: " << std::endl;
    std::cout << "\t mHandCards: [";
    for (int i = 0; i < mHandCards.size() - 1; i++) {
        std::cout << mHandCards[i] << ", ";
    }
    std::cout << mHandCards.back() << "]" << std::endl;

    std::cout << "\t mLastPlayedCard: " << mLastPlayedCard << std::endl;
    std::cout << "\t mCurrentPlayer: " << mCurrentPlayer << std::endl;
    std::cout << "\t mIsInClockwise: " << mIsInClockwise << std::endl;
    std::cout << "\t mCardsNumToDraw: " << mCardsNumToDraw << std::endl;

    std::cout << "\t mPlayerStats: [" << std::endl;
    for (const auto &stat : mPlayerStats) {
        std::cout << "  " << stat << std::endl;
    }
    std::cout << "\t ]" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const PlayerStat& stat)
{
    os << "\t { " << stat.mUsername << ", " << stat.mRemainingHandCardsNum;
    if (stat.mDoPlayInLastRound) {
        os << ", " << stat.mLastPlayedCard;
    }
    os << " }";
    return os;
}
}}
