#include <Commons/Network/NetworkServer.hpp>

#include <utility>

namespace Merrie {

    NetworkServer::NetworkServer(NetworkServerSettings settings)
            : m_settings(std::move(settings)),
              m_endpoint(boost::asio::ip::make_address_v4(m_settings.BindIp), m_settings.BindPort),
              m_acceptor(m_ioContext) {
    }

    NetworkServer::~NetworkServer() {
        m_work.reset();

        // wait for the worker threads to shut down
        Join();
    }

    void NetworkServer::StartServer() {
        M_LOG_TRACE_THIS("Open & bind");

        m_acceptor.open(m_endpoint.protocol());
        m_acceptor.set_option(tcp::acceptor::reuse_address(true));
        m_acceptor.bind(m_endpoint);
        m_acceptor.listen();

        M_LOG_INFO_THIS << "Starting the server with " << m_settings.WorkerThreadCount << " worker threads";

        // start workers
        m_work.reset(new boost::asio::io_context::work(m_ioContext));

        m_workerThreads.reserve(m_settings.WorkerThreadCount);
        for (size_t i = 0; i < m_settings.WorkerThreadCount; i++) {
            m_workerThreads.emplace_back([this]() {
                m_ioContext.run();
            });
        }

        StartAccept();
    }

    void NetworkServer::Join() {
        for (std::thread& thread : m_workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    const NetworkServerSettings& NetworkServer::GetSettings() const noexcept {
        return m_settings;
    }

    const tcp::endpoint& NetworkServer::GetEndpoint() const noexcept {
        return m_endpoint;
    }

    void NetworkServer::StartAccept() {
        std::shared_ptr<NetworkConnection> networkConnection = CreateNetworkConnection(m_ioContext);
        tcp::socket& socket = networkConnection->GetSocket();

        m_acceptor.async_accept(socket, [this, networkConnection = std::move(networkConnection)](const boost::system::error_code& error) mutable {
            // todo handle error

            if (error) {
                return;
            }

            HandleNewConnection(std::move(networkConnection));
            StartAccept();
        });
    }

    void NetworkServer::HandleNewConnection(std::shared_ptr<NetworkConnection> connection) {
        M_LOG_TRACE_THIS("Connection: " << connection->GetSocket().remote_endpoint());

        std::scoped_lock lock(m_connectionsMutex);
        m_connections.emplace_back(connection);
        ReadData(std::move(connection));
    }

    NetworkConnection::NetworkConnection(boost::asio::io_context& ioContext) : m_socket(ioContext) {
    }

    tcp::socket& NetworkConnection::GetSocket() {
        return m_socket;
    }
}