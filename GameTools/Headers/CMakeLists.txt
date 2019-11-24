add_library(Merrie_GameTools_Headers INTERFACE)

target_include_directories(Merrie_GameTools_Headers
        INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/Includes"
            Merrie::Commons::Headers
)

add_library(Merrie::GameTools::Headers ALIAS Merrie_GameTools_Headers)
