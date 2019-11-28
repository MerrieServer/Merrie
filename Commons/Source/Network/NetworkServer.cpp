#include <Commons/Network/NetworkServer.hpp>

#include <utility>

namespace Merrie {

    NetworkServer::NetworkServer(NetworkServerSettings settings)
            : m_settings(std::move(settings)),
              m_endpoint(boost::asio::ip::make_address_v4(m_settings.BindIp), m_settings.BindPort),
              m_acceptor(m_ioContext) {
    }

    NetworkServer::~NetworkServer() {
        Stop();
        Join();
    }

    void NetworkServer::Start() {
        M_ASSERT(!m_running, "Already running");
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

        m_running = true;
        StartAccept();
    }

    void NetworkServer::Join() {
        for (std::thread& thread : m_workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void NetworkServer::Stop() {
        m_work.reset();

        if (m_acceptor.is_open())
            m_acceptor.close();

        m_running = false;
    }

    const NetworkServerSettings& NetworkServer::GetSettings() const noexcept {
        return m_settings;
    }

    const tcp::endpoint& NetworkServer::GetEndpoint() const noexcept {
        return m_endpoint;
    }

    bool NetworkServer::IsRunning() const noexcept {
        return true;
    }

    void NetworkServer::StartAccept() {
        std::shared_ptr<NetworkConnection> networkConnection = CreateNetworkConnection(m_ioContext);
        tcp::socket& socket = networkConnection->GetSocket();

        m_acceptor.async_accept(socket, [this, networkConnection = std::move(networkConnection)](const boost::system::error_code& error) mutable {
            HandleNewConnection(error, std::move(networkConnection));
            StartAccept();
        });
    }

    void NetworkServer::HandleNewConnection(const boost::system::error_code& error, std::shared_ptr<NetworkConnection> connection) {
        // todo handle error
        if (error) {
            return;
        }

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

    bool NetworkConnection::IsValid() {
        return m_socket.is_open();
    }
}