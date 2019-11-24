# Create library
add_library(Merrie_GameServer STATIC
        Network/GameHttpServer.cpp
        GameServer.cpp
        Main.cpp
)

# Find dependencies
target_link_libraries(Merrie_GameServer
        PUBLIC
            Merrie::GameServer::Headers
            Merrie::Commons
)

# Add alias
add_library(Merrie::GameServer ALIAS Merrie_GameServer)