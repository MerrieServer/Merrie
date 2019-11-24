add_library(Merrie_GameServer_Headers INTERFACE)

target_include_directories(Merrie_GameServer_Headers
        INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/Includes"
            Merrie::Commons::Headers
)

add_library(Merrie::GameServer::Headers ALIAS Merrie_GameServer_Headers)
