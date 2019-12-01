# Create library
add_library(Merrie_GameServer STATIC
        Network/GameHttpServer.cpp
        Network/Packets.cpp
        GameServer.cpp
        Main.cpp
        Player.cpp
)

# Find dependencies
find_package(yaml-cpp CONFIG REQUIRED)

target_link_libraries(Merrie_GameServer
        PUBLIC
            Merrie::GameServer::Headers
            Merrie::Commons
        PRIVATE
            yaml-cpp
)

# Add alias
add_library(Merrie::GameServer ALIAS Merrie_GameServer)