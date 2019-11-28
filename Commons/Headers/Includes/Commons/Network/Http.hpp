#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_NETWORK_HTTP_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_NETWORK_HTTP_HPP

#include "../Commons.hpp"
#include "../Time.hpp"
#include "NetworkServer.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace Merrie {

    class HttpServer; // Forward declaration
    class HttpConnection;


    // ================================================================================
    // =  URL Decoding                                                                =
    // ================================================================================

    /**
     * Thrown when there is a problem decoding URL string
     */
    M_DECLARE_EXCEPTION(UrlDecodeException);

    /**
     * Represents a decoded URL path with parameters.
     */
    struct DecodedUrl {
        std::string Path{};
        std::map<std::string, std::string> Parameters{};
    };

    /**
     * Decodes an URL string path into path and parameters.
     *
     * \throw  UrlDecodeException
     */
    [[nodiscard]] DecodedUrl DecodeUrlQueryString(std::string_view queryString);


    // ================================================================================
    // =   HttpServer                                                                 =
    // ================================================================================

    /**
     * Shorter namespace definition for the boost http namespace
     */
    namespace http = boost::beast::http;

    /**
     * Settings of an HTTP server
     */
    struct HttpServerSettings {
        /**
         * Settings for the underlying NetworkServer
         */
        NetworkServerSettings NetworkServerSettings;

        /**
         * Should KeepAlive be enabled.
         */
        bool AllowKeepAlive{};

        /**
         * General timeout for requestes in seconds.
         */
        uint16_t RequestTimeout {};

        /**
         * Timeout for the keep alive in seconds.
         */
        uint16_t KeepAliveTimeout{};

        /**
         * KeepAlive max - indicates the maximum number of requests that can be sent on this connection before closing it.
         */
        uint16_t KeepAliveMax{};
    };

    /**
     * A client connection, connected to a HttpServer
     */
    class HttpConnection : public NetworkConnection, public std::enable_shared_from_this<HttpConnection> {
        public: // Constructors & destructors
            NON_COPYABLE(HttpConnection);
            NON_MOVEABLE(HttpConnection);

            /**
             * Creates a new HttpConnection
             *
             * @param ioContext I/O context to be used by this connection
             * @param server server that owns this connection
             */
            HttpConnection(boost::asio::io_context& ioContext, HttpServer* server);

            /**
             * Gets the last request that this connection received.
             */
            [[nodiscard]] http::request<boost::beast::http::string_body>& GetRequest() noexcept;

            /**
             * Gets the cached response, it can be send using SendResponse()
             */
            [[nodiscard]] http::response<boost::beast::http::string_body>& GetResponse() noexcept;

            /**
             * Sends the cached response (the one returned by GetResponse()) to the client
             */
            void SendResponse();

        public: // Overriden functions
            bool IsValid() override;

        protected: // Friend methods
            friend class HttpServer;

            void ReadData(std::shared_ptr<NetworkConnection> connectionOwnership);

        private: // Private methods
            void SetTimeout();

        private: // Private fields
            bool m_keepAlive = false;
            bool m_invalidated = false;
            DefaultClock::time_point m_timeout;
            HttpServer* m_server;
            boost::beast::flat_static_buffer<8192> m_buffer;
            http::request<http::string_body> m_request{};
            http::response<http::string_body> m_response{};
    };

    /**
     * Represents a simple HTTP based of on a NetworkServer
     */
    class HttpServer : public NetworkServer {
        public: // Constructors & destructors
            NON_COPYABLE(HttpServer);
            NON_MOVEABLE(HttpServer);

            /**
             * Creates a new HttpServer with the given settings
             */
            explicit HttpServer(HttpServerSettings settings);

        public: // Public methods

            /**
             * Gets the settings of this HttpServer
             */
            [[nodiscard]] const HttpServerSettings& GetHttpSettings() const;

        protected: // Protected methods
            void ReadData(std::shared_ptr<NetworkConnection> connection) override;

            std::shared_ptr<NetworkConnection> CreateNetworkConnection(boost::asio::io_context& context) override;

            virtual void HandleRequest(std::shared_ptr<HttpConnection> connection) = 0;

        protected: // Friend methods
            friend class HttpConnection;

        private: // Private fields
            const HttpServerSettings m_settings;
            const std::string m_keepAliveHeader;
    };
}

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_NETWORK_HTTP_HPP
