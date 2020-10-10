#include "player.h"

namespace UNO { namespace Game {

Player::Player(std::string username, std::string host, std::string port)
    : mUsername(username), mClient(host, port), 
    mUIManager(std::make_unique<UIManager>(mPlayerStats))
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
    while (!mGameEnds) {
        if (mCurrentPlayer == 0) {
            char inputBuffer[10];
            std::cout << "Now it's your turn." << std::endl;
            std::cout << *mHandCards << std::endl;
            while (true) {
                std::cout << "Input (D)raw, (S)kip or (P)lay <card_index>:" << std::endl;
                std::cin.getline(inputBuffer, 10);
                std::string input(inputBuffer);
                if (input == "D") {
                    // Draw
                    mClient.DeliverInfo<DrawInfo>(mCardsNumToDraw);

                    // wait for draw rsp msg
                    std::unique_ptr<DrawRspInfo> info = mClient.ReceiveInfo<DrawRspInfo>();
                    std::cout << *info << std::endl;

                    mHandCards->Draw(info->mCards);
                    break;
                }
                else if (input == "S") {
                    // Skip
                    mClient.DeliverInfo<SkipInfo>();
                    break;
                }
                else if (input[0] == 'P') {
                    // Play
                    int cardIndex = std::stoi(input.substr(1));
                    char nextColor = ' ';
                    Card cardToPlay = mHandCards->At(cardIndex);
                    if (cardToPlay.mColor == CardColor::BLACK) {
                        while (!std::set<char>{'R', 'Y', 'G', 'B'}.count(nextColor)) {
                            std::cout << "Specify the next color (R/Y/G/B): " << std::endl;
                            std::cin.getline(inputBuffer, 10);
                            nextColor = *inputBuffer;
                        }
                    }
                    if (mHandCards->Play(cardIndex, mLastPlayedCard)) {
                        if (nextColor == ' ') {
                            mClient.DeliverInfo<PlayInfo>(cardToPlay);
                        }
                        else {
                            mClient.DeliverInfo<PlayInfo>(cardToPlay, Card::FromChar(nextColor));
                        }
                        // the index of player himself is 0
                        UpdateStateAfterPlay(0, cardToPlay);
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
        mCurrentPlayer = mIsInClockwise ? WrapWithPlayerNum(mCurrentPlayer + 1) 
            : WrapWithPlayerNum(mCurrentPlayer - 1);
        // PrintLocalState();

        mUIManager->Render();
    }
}

void Player::HandleDraw(const std::unique_ptr<DrawInfo> &info)
{
    std::cout << *info << std::endl;
    mPlayerStats[info->mPlayerIndex].UpdateAfterDraw(info->mNumber);
}

void Player::HandleSkip(const std::unique_ptr<SkipInfo> &info)
{
    std::cout << *info << std::endl;
    mPlayerStats[info->mPlayerIndex].UpdateAfterSkip();
}

void Player::HandlePlay(const std::unique_ptr<PlayInfo> &info)
{
    std::cout << *info << std::endl;
    UpdateStateAfterPlay(info->mPlayerIndex, info->mCard);
}

void Player::UpdateStateAfterPlay(int playerIndex, Card cardPlayed)
{
    PlayerStat &stat = mPlayerStats[playerIndex];
    stat.UpdateAfterPlay(cardPlayed);
    if (stat.GetRemainingHandCardsNum() == 0) {
        Win(playerIndex);
    }

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

void Player::Win(int playerIndex)
{
    mGameEnds = true;
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

}}
