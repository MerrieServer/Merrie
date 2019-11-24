# Create library
add_library(Merrie_GameTools STATIC
        Main.cpp
)

# Find dependencies
target_link_libraries(Merrie_GameTools
        PUBLIC
            Merrie::Commons
)

# Add alias
add_library(Merrie::GameTools ALIAS Merrie_GameTools)