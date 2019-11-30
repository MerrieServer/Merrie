#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_NETWORK_NETWORKSERVER_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_NETWORK_NETWORKSERVER_HPP

#include "../Commons.hpp"
#include "../Logging.hpp"
#include "Network.hpp"

#include <boost/asio.hpp>
#include <optional>

namespace Merrie {
    class NetworkWorker; // Forward declaration

    /**
     * Shorter definition for boost tcp class
     */
    using tcp = boost::asio::ip::tcp;

    /**
     * Settings of a network server
     */
    struct NetworkServerSettings {

        /**
         * IP to which the server should bind to.
         */
        std::string BindIp{};

        /**
         * Port to which the server should bind to.
         */
        NetworkPort BindPort{};

        /**
         * How many worker threads should be spawned for this server.
         */
        size_t WorkerThreadCount{};
    };

    /**
     * A client connection, connected to a NetworkServer
     */
    class NetworkConnection {
        public: // Constructors & destructors
            NON_COPYABLE(NetworkConnection);
            NON_MOVEABLE(NetworkConnection);

            /**
             * Creates a new NetworkConnection with the given ioContext that will be used for intiializing socket.
             */
            explicit NetworkConnection(boost::asio::io_context& ioContext);

        public: // Public methods
            /**
             * Determines whether or not the connection is still valid.
             */
            virtual bool IsValid();

            /**
             * Get remote endpoint of this connection, if it exists
             */
            [[nodiscard]] const std::optional<tcp::endpoint>& GetRemoteEndpoint() const;
        protected: // Friend methods
            tcp::socket& GetSocket();

            friend class NetworkServer;

        private: // Private fields
            tcp::socket m_socket;
            std::optional<tcp::endpoint> m_remoteEndpoint;
    };

    /**
     * A simple Network server implementation.
     */
    class NetworkServer {
        public: // Constructors & destructors
            NON_COPYABLE(NetworkServer);
            NON_MOVEABLE(NetworkServer);

            /**
             * Creates a new NetworkServer with the given settings
             */
            explicit NetworkServer(NetworkServerSettings settings);

            virtual ~NetworkServer();

        public: // Public methods

            /**
             * Binds the server to the configured IP and port, starts listening and starts worker threads.
             */
            void Start();

            /**
             * Blocks the current thread until all of this server's network threads exit.
             */
            void Join();

            /**
             * Stops the servers worker threads and cleans up the socket.
             */
            void Stop();

            /**
             * Gets the setting of this NetworkServer
             */
            [[nodiscard]] const NetworkServerSettings& GetSettings() const noexcept;

            /**
             * Gets the endpoint that the server is bound to/will bind to
             */
            [[nodiscard]] const tcp::endpoint& GetEndpoint() const noexcept;

            /**
             * Indicates whether or not this server is running properly.
             */
            [[nodiscard]] bool IsRunning() const noexcept;

        protected: // Protected methods
            virtual std::shared_ptr<NetworkConnection> CreateNetworkConnection(boost::asio::io_context& context) = 0;

            virtual void ReadData(std::shared_ptr<NetworkConnection> connection) = 0;

        private: // Private methods
            void StartAccept();

            void HandleNewConnection(const boost::system::error_code& error, std::shared_ptr<NetworkConnection> connection);

        private: // Private fields
            const NetworkServerSettings m_settings;
            const tcp::endpoint m_endpoint;

            bool m_running = false;
            boost::asio::io_context m_ioContext{};
            std::unique_ptr<boost::asio::io_context::work> m_work;
            tcp::acceptor m_acceptor;
            std::vector<std::thread> m_workerThreads;
            std::mutex m_connectionsMutex;
            std::vector<std::shared_ptr<NetworkConnection>> m_connections;

            M_DECLARE_LOGGER_EX("Server " + m_settings.BindIp + ":" + std::to_string(m_settings.BindPort));
    };
}

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_NETWORK_NETWORKSERVER_HPP
