#ifndef MERRIE_GAMESERVER_SOURCE_NETWORK_PLAYERCONNECTION_HPP
#define MERRIE_GAMESERVER_SOURCE_NETWORK_PLAYERCONNECTION_HPP
#pragma once

#include <Commons/Time.hpp>
#include "GameServer.hpp"
#include <shared_mutex>

namespace Merrie {
    /*
     * TODO Docs
     */

    enum class InitLevel {
            None = 0,
            Level1 = 1,
            Level2 = 2,
            Level3 = 3,
            Level4 = 4,
            FullyInitialized = 4
    };

    class Player {
        public:
            NON_COPYABLE(Player);
            NON_MOVEABLE(Player);

            explicit Player(uint64_t aid);

        public:
            [[nodiscard]] uint64_t GetAid() const noexcept;

            [[nodiscard]] const std::string& GetCharacterName() const noexcept;

            void SetTimeout();

            [[nodiscard]] bool IsOnline() const noexcept;

            [[nodiscard]] bool IsInitialized() const noexcept;

            [[nodiscard]] const DefaultClock::time_point& GetTimeout() const noexcept;

            [[nodiscard]] InitLevel GetInitLevel() const noexcept;

            void SetInitLevel(InitLevel initLevel) noexcept;

            [[nodiscard]] uint32_t GetBrowserToken() const noexcept;

            void SetBrowserToken(uint32_t browserToken) noexcept;

            [[nodiscard]] const std::string& GetKickMessage() const noexcept;

            void SetKickMessage(const std::string& kickMessage) noexcept;

            [[nodiscard]] std::shared_mutex& GetDataMutex() const noexcept;

            void Kick(std::string_view message);

            Logger& GetLogger() const;
        private:
            const uint64_t m_aid;
            const std::string m_username;

            mutable std::shared_mutex m_dataMutex;
            InitLevel m_initLevel = InitLevel::None;
            uint32_t m_browserToken = 0;
            DefaultClock::time_point m_timeout{};
            std::string m_kickMessage{};

            M_DECLARE_LOGGER_EX("Player#"s + std::to_string(m_aid));
            // TODO: Account
    };

}


#endif //MERRIE_GAMESERVER_SOURCE_NETWORK_PLAYERCONNECTION_HPP
