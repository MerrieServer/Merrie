#include <GameServer/Player.hpp>
#include <mutex>

namespace Merrie {

    Player::Player(uint64_t aid) :
            m_aid(aid),
            m_username("User#" + std::to_string(aid)) {

        SetTimeout();
    }

    uint64_t Player::GetAid() const noexcept {
        return m_aid;
    }

    const std::string& Player::GetCharacterName() const noexcept {
        return m_username;
    }

    void Player::SetTimeout() {
        m_timeout = PointInFuture<std::chrono::seconds>(10); // todo: configurable
    }

    bool Player::IsOnline() const noexcept {
        return m_initLevel > InitLevel::None;
    }

    bool Player::IsInitialized() const noexcept {
        return m_initLevel == InitLevel::FullyInitialized;
    }

    InitLevel Player::GetInitLevel() const noexcept {
        return m_initLevel;
    }

    void Player::SetInitLevel(InitLevel initLevel) noexcept {
        m_initLevel = initLevel;
    }

    uint32_t Player::GetBrowserToken() const noexcept {
        return m_browserToken;
    }

    void Player::SetBrowserToken(uint32_t browserToken) noexcept {
        m_browserToken = browserToken;
    }

    const DefaultClock::time_point& Player::GetTimeout() const noexcept {
        return m_timeout;
    }

    std::shared_mutex& Player::GetDataMutex() const noexcept {
        return m_dataMutex;
    }

    [[nodiscard]] const std::string& Player::GetKickMessage() const noexcept {
        return m_kickMessage;
    }

    void Player::SetKickMessage(const std::string& kickMessage) noexcept {
        m_kickMessage = kickMessage;
    }

    void Player::Kick(std::string_view message) {
        std::scoped_lock lock(m_dataMutex);
        m_kickMessage = message;
        m_initLevel = InitLevel::None;
    }

    Logger& Player::GetLogger() const {
        return m_logger;
    }
}