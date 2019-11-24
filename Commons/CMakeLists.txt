# Subdirectories
add_subdirectory("Headers")
add_subdirectory("Source")

if (MERRIE_DO_UNIT_TESTS)
    add_subdirectory("Tests")
endif()