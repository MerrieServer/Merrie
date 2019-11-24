add_library(Merrie_Commons_Headers INTERFACE)

# Includes
target_include_directories(Merrie_Commons_Headers
        INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/Includes"
)


# Platform-specific definitions
if (WIN32)
    target_compile_definitions(Merrie_Commons_Headers INTERFACE -DM_PLATFORM_WINDOWS)
endif ()

if (UNIX)
    target_compile_definitions(Merrie_Commons_Headers INTERFACE -DM_PLATFORM_UNIX)
endif ()

if (APPLE)
    target_compile_definitions(Merrie_Commons_Headers INTERFACE -DM_PLATFORM_MACOS)
endif ()

# Maybe do it conditional in the future?
target_compile_definitions(Merrie_Commons_Headers
        INTERFACE
            -DM_ENABLE_TRACE
            -DM_ENABLE_DEBUG
)

# Find dependencies
# Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.69 REQUIRED COMPONENTS system log)

target_link_libraries(Merrie_Commons_Headers
        INTERFACE
            Boost::system
            Boost::log
)

# OpenSSL
if (MERRIE_USE_OPENSSL)
    message(STATUS "Using OpenSSL as requested")

    set(OPENSSL_USE_STATIC_LIBS ON)
    find_package(OpenSSL 1.0.0 REQUIRED)

    if (TARGET OpenSSL::SSL)
        message(STATUS "OpenSSL has SSL")
        target_link_libraries(Merrie_Commons_Headers INTERFACE OpenSSL::SSL)
        target_compile_definitions(Merrie_Commons_Headers INTERFACE -DM_HAS_OPENSSL_SSL)
    else()
        message(WARNING "OpenSSL has no SSL")
    endif()

    if (TARGET OpenSSL::Crypto)
        message(STATUS "OpenSSL has Crypto")
        target_link_libraries(Merrie_Commons_Headers INTERFACE OpenSSL::Crypto)
        target_compile_definitions(Merrie_Commons_Headers INTERFACE -DM_HAS_OPENSSL_CRYPTO)
    else()
        message(WARNING "OpenSSL has no Crypto")
    endif()
endif()

add_library(Merrie::Commons::Headers ALIAS Merrie_Commons_Headers)
