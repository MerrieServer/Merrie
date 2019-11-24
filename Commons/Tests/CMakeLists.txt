message(STATUS "Unit tests enabled")
enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(Merrie_Commons_Test
        Crypto/TestDigest.cpp
        Network/TestHttp.cpp
        TestCommons.cpp
        TestTicker.cpp
        TestTime.cpp
)

target_link_libraries(Merrie_Commons_Test
        PRIVATE
            Merrie::Commons
            GTest::gtest
            GTest::gtest_main
)

add_test(Merrie_Commons_Test Merrie_Commons_Test)