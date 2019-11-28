#ifndef MERRIE_GAMESERVER_SOURCE_NETWORK_PLAYERCONNECTION_HPP
#define MERRIE_GAMESERVER_SOURCE_NETWORK_PLAYERCONNECTION_HPP
#pragma once

#include "GameServer.hpp"

namespace Merrie {
    /*
     * TODO Docs
     */

    class Player {
        public:
            NON_COPYABLE(Player);
            NON_MOVEABLE(Player);

            explicit Player(uint64_t aid);

        public:
            [[nodiscard]] uint64_t GetAid() const noexcept;

            [[nodiscard]] const std::string& GetCharacterName() const noexcept;

        private:
            const uint64_t m_aid;
            const std::string m_username;

            // TODO: Account
    };

}


#endif //MERRIE_GAMESERVER_SOURCE_NETWORK_PLAYERCONNECTION_HPP
