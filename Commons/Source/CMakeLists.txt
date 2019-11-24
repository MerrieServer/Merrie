# Create library
add_library(Merrie_Commons STATIC
        Crypto/Digest.cpp
        Crypto/OpenSSL.cpp
        Network/Http.cpp
        Network/NetworkServer.cpp
        Logging.cpp
        Ticker.cpp
)

target_link_libraries(Merrie_Commons
        PUBLIC
            Merrie::Commons::Headers
)

# Add alias
add_library(Merrie::Commons ALIAS Merrie_Commons)