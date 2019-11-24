add_executable(Merrie_GameTools_Executable Main.cpp)

target_link_libraries(Merrie_GameTools_Executable
        PRIVATE
            Merrie::GameTools
)

add_executable(Merrie::GameTools::Executable ALIAS Merrie_GameTools_Executable)