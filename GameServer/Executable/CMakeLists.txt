add_executable(Merrie_GameServer_Executable Main.cpp)

target_link_libraries(Merrie_GameServer_Executable
        PRIVATE
            Merrie::GameServer
)

add_executable(Merrie::GameServer::Executable ALIAS Merrie_GameServer_Executable)