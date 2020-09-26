#include "info.h"

namespace UNO { namespace Game {

using namespace Network;

void JoinGameInfo::Serialize(uint8_t *buffer) const
{
    JoinGameMsg *msg = reinterpret_cast<JoinGameMsg *>(buffer);
    msg->mType = MsgType::JOIN_GAME;
    msg->mLen = mUsername.size();
    std::strcpy(msg->mUsername, mUsername.c_str());
}

JoinGameInfo JoinGameInfo::Deserialize(const uint8_t *buffer)
{
    const JoinGameMsg *msg = reinterpret_cast<const JoinGameMsg *>(buffer);
    JoinGameInfo info;
    info.mUsername = msg->mUsername;
    return info;
}

void GameStartInfo::Serialize(uint8_t *buffer) const
{
    GameStartMsg *msg = reinterpret_cast<GameStartMsg *>(buffer);
    msg->mType = MsgType::GAME_START;

    std::string usernames{};
    std::for_each(mUsernames.begin(), mUsernames.end(),
        [&usernames](const std::string &username) {
            // ' ' as delimiter of usernames
            usernames.append(username).push_back(' ');
        }
    );
    msg->mLen = sizeof(Card) * 8 + sizeof(int) + usernames.size();

    std::copy(mInitHandCards.begin(), mInitHandCards.end(), msg->mInitHandCards);
    msg->mFlippedCard = mFlippedCard;
    msg->mFirstPlayer = mFirstPlayer;
    std::strcpy(msg->mUsernames, usernames.c_str());
}

GameStartInfo GameStartInfo::Deserialize(const uint8_t *buffer)
{
    const GameStartMsg *msg = reinterpret_cast<const GameStartMsg *>(buffer);
    GameStartInfo info;

    std::copy(std::begin(msg->mInitHandCards), std::end(msg->mInitHandCards),
            info.mInitHandCards.begin());
    info.mFlippedCard = msg->mFlippedCard;
    info.mFirstPlayer = msg->mFirstPlayer;
    std::string usernames(msg->mUsernames);
    while (!usernames.empty()) {
        int pos = usernames.find(' ');
        info.mUsernames.emplace_back(usernames, pos);
        usernames.erase(0, pos + 1);
    }

    return info;
}

}}