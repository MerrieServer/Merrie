#ifndef MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_PACKET_HANDLERS_HPP
#define MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_PACKET_HANDLERS_HPP

#include "../GameServer.hpp"

#include <Commons/Network/Http.hpp>
#include <nlohmann/json.hpp>

namespace Merrie {
    class Player; // Player.hpp

    /*
 * TODO Docs
 */

    class OutgoingPacket {
        public:
            TRIVIALLY_MOVEABLE(OutgoingPacket);
            TRIVIALLY_COPYABLE(OutgoingPacket);

            OutgoingPacket();

            [[nodiscard]] nlohmann::json& GetJson() noexcept;
        private:
            nlohmann::json m_json;
    };

    struct IncomingPacket {
        std::shared_ptr<Player> Player_;
        std::string Action;
        std::map<std::string, std::string> Parameters;
    };

    enum class HandleResult {
            Ignored, // ignored by the handler
            ContinueHandling, // handled, but handling can continue
            StopHandling, // handling, but handling should stop immediatly with a fail
    };

    using PacketHandler = std::function<HandleResult(const std::shared_ptr<Player>& player, const IncomingPacket& in, OutgoingPacket& out)>;

    enum class RunMode {
            Sync,
            Async
    };

    struct PacketHandlerData {
        RunMode Mode;
        std::vector<std::string> ActionFilters;
        PacketHandler Handler;
    };


    [[nodiscard]] const std::vector<std::shared_ptr<PacketHandlerData>>& GetRegisteredPacketHandlers() noexcept;

    [[nodiscard]] const std::vector<std::shared_ptr<PacketHandlerData>>& GetRegisteredSyncPacketHandlers() noexcept;

    [[nodiscard]] const std::vector<std::shared_ptr<PacketHandlerData>>& GetRegisteredAsyncPacketHandlers() noexcept;

    void RegisterPacketHandler(RunMode mode, std::vector<std::string> actionFilters, PacketHandler handler) noexcept;
}

#endif //MERRIE_GAMESERVER_HEADERS_GAMESERVER_NETWORK_GAMEHTTPSERVER_HPP
