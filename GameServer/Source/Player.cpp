#include <GameServer/Player.hpp>

namespace Merrie {

    Player::Player(uint64_t aid) :
            m_aid(aid),
            m_username("User#" + std::to_string(aid)) {

    }

    uint64_t Player::GetAid() const noexcept {
        return m_aid;
    }

    const std::string& Player::GetCharacterName() const noexcept {
        return m_username;
    }
}