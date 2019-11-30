#include <GameServer/Network/Packets.hpp>

#include <Commons/Containers.hpp>
#include <Commons/Random.hpp>
#include <Commons/Time.hpp>
#include <GameServer/Player.hpp>
#include <utility>
#include <boost/lexical_cast.hpp>

namespace Merrie {

    OutgoingPacket::OutgoingPacket() {
    }

    nlohmann::json& OutgoingPacket::GetJson() noexcept {
        return m_json;
    }

    namespace {
        std::vector<std::shared_ptr<PacketHandlerData>> g_packetHandlers;
        std::vector<std::shared_ptr<PacketHandlerData>> g_asyncPacketHandlers;
        std::vector<std::shared_ptr<PacketHandlerData>> g_syncPacketHandlers;
    }

    const std::vector<std::shared_ptr<PacketHandlerData>>& GetRegisteredPacketHandlers() noexcept {
        return g_packetHandlers;
    }

    const std::vector<std::shared_ptr<PacketHandlerData>>& GetRegisteredSyncPacketHandlers() noexcept {
        return g_syncPacketHandlers;
    }

    const std::vector<std::shared_ptr<PacketHandlerData>>& GetRegisteredAsyncPacketHandlers() noexcept {
        return g_asyncPacketHandlers;
    }

    inline void RegisterPacketHandler(RunMode mode, std::vector<std::string> actionFilters, PacketHandler handler) noexcept {
        std::shared_ptr<PacketHandlerData> ref = g_packetHandlers.emplace_back(std::make_shared<PacketHandlerData>(PacketHandlerData{mode, std::move(actionFilters), std::move(handler)}));

        if (ref->Mode == RunMode::Sync)
            g_syncPacketHandlers.emplace_back(std::move(ref));
        else if (ref->Mode == RunMode::Async)
            g_asyncPacketHandlers.emplace_back(std::move(ref));
        else
            M_FAIL("ref.Mode = ???");
    }

    namespace {
        void RegisterStandardHandlers() noexcept {
            RegisterPacketHandler(RunMode::Async, {}, [](const IncomingPacket& in, OutgoingPacket&) -> HandleResult {
                // browser_token and initlvl check task
                std::scoped_lock lock(in.Player->GetDataMutex());

                // check initlvl
                if (in.Action != "init" && in.Player->GetInitLevel() != InitLevel::FullyInitialized) {
                    return HandleResult::StopHandling;
                }

                // check browser token
                if (in.Player->GetBrowserToken() != 0) {
                    // we don't care about browser_token if its the first init request
                    if (!(in.Action == "init" && FindInMap(in.Parameters, "initlvl"s) == "1")) {
                        const auto browserTokenParam = FindInMap(in.Parameters, "browser_token"s);
                        uint32_t browserToken;
                        if (!browserTokenParam || !boost::conversion::try_lexical_convert(browserTokenParam.value(), browserToken) || browserToken != in.Player->GetBrowserToken()) {
                            return HandleResult::StopHandling;
                        }
                    }
                }

                return HandleResult::Ignored;
            });


            RegisterPacketHandler(RunMode::Sync, {"init"}, [](const IncomingPacket& in, OutgoingPacket& out) -> HandleResult {
                std::unique_lock lock(in.Player->GetDataMutex());

                // 'init' action handler
                const auto initLvl = FindInMap(in.Parameters, "initlvl"s);

                InitLevel requestedInitLevel;
                if (initLvl == "1") {
                    requestedInitLevel = InitLevel::Level1;
                } else if (initLvl == "2") {
                    requestedInitLevel = InitLevel::Level2;
                } else if (initLvl == "3") {
                    requestedInitLevel = InitLevel::Level3;
                } else if (initLvl == "4") {
                    requestedInitLevel = InitLevel::Level4;
                } else {
                    return HandleResult::StopHandling;
                }

                const auto nextInitLevel = static_cast<InitLevel>(static_cast<int>(in.Player->GetInitLevel()) + 1);

                // you can  always request initlvl 1
                if (requestedInitLevel != InitLevel::Level1 && requestedInitLevel != nextInitLevel) {
                    return HandleResult::StopHandling;
                }

                switch (requestedInitLevel) {
                    case InitLevel::Level1: {
                        static RandomNumberGenerator<uint32_t> c_browserTokenGenerator = CreateRandomNumberGenerator<uint32_t>();
                        in.Player->SetBrowserToken(c_browserTokenGenerator());

                        out.GetJson() = {
                                {"browser_token", in.Player->GetBrowserToken()},
                                {"qtrack",        {"*"}},
                                {"priv_world",    0},
                                {"wanted_show",   1},
                                {"tutorial",      0},
                                {"worldname",     "Merrie"},
                                {"h",
                                                  {
                                                   {"id", in.Player->GetAid()},
                                                          {"blockade", 0},
                                                          {"uprawnienia", 0},
                                                          {"ap", 0},
                                                          {"bagi", 1},
                                                          {"bint", 1},
                                                          {"bstr", 1},
                                                          {"credits", 0},
                                                          {"runes", 0},
                                                          {"exp", 0},
                                                          {"gold", 0},
                                                          {"goldlim", 100000000000},
                                                          {"healpower", 0},
                                                          {"honor", 812},
                                                          {"img", "/paid/zakon_rm5.gif"},
                                                          {"lvl", 10000},
                                                          {"mails", 0},
                                                          {"mails_all", 0},
                                                          {"mails_last", ""},
                                                          {"mpath", "http://classic.margonem.pl/"},
                                                          {"nick", in.Player->GetCharacterName()},
                                                          {"opt", 0},
                                                          {"prof", "w"},
                                                          {"ttl_value", 0},
                                                          {"ttl_end", 1577836800},
                                                          {"ttl_del", 0},
                                                          {"pvp", 0},
                                                          {"ttl", 300},
                                                          {"x", 10},
                                                          {"y", 10},
                                                          {"dir", 1},
                                                          {"stasis", 0},
                                                          {"bag", 0},
                                                          {"party", 0},
                                                          {"trade", 0},
                                                          {"wanted", 0},
                                                          {"stamina", 50},
                                                          {"stamina_ts", 1577836800},
                                                          {"stamina_renew_sec", 0},
                                                          {"cur_skill_set", 1},
                                                          {"cur_battle_set", 1},
                                                          {"attr", 1},
                                                          {
                                                                  "warrior_stats",
                                                                  {
                                                                          {"hp", 50},
                                                                          {"maxhp", 50},
                                                                          {"st", 12},
                                                                          {"ag", 34},
                                                                          {"it", 56},
                                                                          {"sa", 12.34},
                                                                          {"crit", 56.78},
                                                                          {"ac", 1234},
                                                                          {"resfire", 12},
                                                                          {"resfrost", 34},
                                                                          {"reslight", 56},
                                                                          {"act", 78},
                                                                          {"dmg", 1234},
                                                                          {"dmgc", 567},
                                                                          {"evade", {50, 5.00}},
                                                                          {"lowcrit", 1},
                                                                          {"heal", 234},
                                                                          {"critmval", 5.67},
                                                                          {"critmval_f", 6.78},
                                                                          {"critmval_c", 7.89},
                                                                          {"critmval_l", 8.90},
                                                                          {"mana", 123},
                                                                          {"acmdmg", 4},
                                                                          {"managain", 56},
                                                                          {"blok", 789},
                                                                          {"critval", 10.11},
                                                                          {"energy", 1213},
                                                                          {"lowevade", 14},
                                                                          {"energygain", 50}
                                                                  }
                                                          }
                                                  }
                                }
                        };
                        break;
                    }
                    case InitLevel::Level2: {
                        out.GetJson() = {
                                {"town",
                                              {
                                                      {"id",      1},
                                                      {"mainid", 0},
                                                      {"x",     96},
                                                      {"y", 100},
                                                      {"file", "ithan.5.png"},
                                                      {"name", "Ithan"},
                                                      {"pvp", 2},
                                                      {"mode", 0},
                                                      {"water", ""},
                                                      {"bg", "007.jpg"},
                                                      {"welcome", ""},
                                                      {"visibility", 0}
                                              }},
                                {"gw2",       nlohmann::json::array()},
                                {"townname",  nlohmann::json::object()},
                                {"worldname", "pandora"},
                                {"cl",        ""},
                                {
                                 "barters",
                                              {
                                                      {"premium", 0},
                                                      {"dragon", 0},
                                                      {"event", 0}
                                              },
                                }
                        };
                        break;
                    }
                    case InitLevel::Level3: {
                        break;
                    }
                    case InitLevel::Level4: {
                        out.GetJson() = {
                                {"c",
                                        {
                                                {"0",
                                                        {
                                                                {"k", 3},
                                                                {"n", "System"},
                                                                {"i", ""},
                                                                {"nd", in.Player->GetCharacterName()},
                                                                {"t", "Siemano kolano"},
                                                                {"s", "sys_info"},
                                                                {"ts", static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(DefaultClock::now().time_since_epoch()).count()) /
                                                                       1000.0f}
                                                        }
                                                }
                                        }
                                }
                        };
                        break;
                    }
                    default:
                        M_FAIL("how did we even get here");
                }


                in.Player->SetInitLevel(requestedInitLevel);

                return HandleResult::ContinueHandling;
            });

            // Finishing up tasks
            RegisterPacketHandler(RunMode::Sync, {}, [](const IncomingPacket& in, OutgoingPacket& out) -> HandleResult {
                std::shared_lock lock(in.Player->GetDataMutex());

                if (out.GetJson().find("e") == out.GetJson().end()) {
                    out.GetJson()["e"] = "ok";
                }

                if (in.Player->IsInitialized()) {
                    out.GetJson()["ev"] = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / 1000.0;
                }

                return HandleResult::ContinueHandling;
            });

        }

        M_INITIALIZER(RegisterStandardHandlers)
    }
}