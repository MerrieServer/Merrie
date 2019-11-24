# Subdirectories
add_subdirectory("Headers")
add_subdirectory("Source")
add_subdirectory("Executable")


if (MERRIE_DO_UNIT_TESTS)
    add_subdirectory("Tests")
endif()