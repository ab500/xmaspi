#include "SocketServer.h"

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <algorithm>

SocketCommand::SocketCommand(uint8_t commandId, uint8_t payloadLen, const uint8_t* payloadPtr)
    : m_commandId(commandId)
    , m_payloadLen(payloadLen)
    , m_payloadBuf(nullptr)
{
    m_payloadBuf = new uint8_t[payloadLen];
}

SocketCommand::~SocketCommand()
{
    if (m_payloadBuf) {
        delete[] m_payloadBuf;
        m_payloadBuf = nullptr;
    }
}

SocketServer::SocketServer()
    : m_isRunning(false)
    , m_sockfd(-1)
{
}

SocketServer::~SocketServer()
{
    StopListening();
    // Clear the queue of accetped connections.
}

void SocketServer::BeginListening()
{
    std::lock_guard<std::mutex> lock(m_listenerLock);

    if (!m_isRunning) {
        m_isRunning = true;
        m_listenerThread = std::thread(&SocketServer::RunListener, this);
    }   
}

void SocketServer::StopListening()
{
    std::cout << "Stopping..." << std::endl;
    std::lock_guard<std::mutex> lock(m_listenerLock);
    CloseSocket();
    if (m_isRunning) {
        m_isRunning = false;
        m_listenerThread.join();
    }
}

void SocketServer::SetDispatcherCallback(std::function<void(const SocketCommand&)> func)
{
}

void SocketServer::RunListener()
{
    std::cout << "Initializing server on port " << c_port << std::endl;
    SetupSocket();
    
    std::cout << "Waiting for connections..." << std::endl;
    while (m_isRunning)
    {
        AcceptConnection();
    }

    std::cout << "Socket closed by SocketServer." << std::endl;
}

void SocketServer::SetupSocket()
{
    int status = 0;
    int sockfd = -1;
    struct addrinfo hints = {};
    struct addrinfo* servInfo = nullptr;
    char portStr[10] = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    sprintf(portStr, "%d", c_port);

    status = getaddrinfo(NULL, portStr, &hints, &servInfo);
    if (status < 0) {
        throw std::runtime_error("Failed to get address info.");
    }
   
    sockfd = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
    if (sockfd == -1) {
        throw std::runtime_error("Failed to create socket.");
    }
    
    status = bind(sockfd, servInfo->ai_addr, servInfo->ai_addrlen);
    if (status < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to bind to port.");
    }

    freeaddrinfo(servInfo);
    
    status = listen(sockfd, c_backlog);
    if (status < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to begin listening");
    }
    
    m_sockfd = sockfd;
}

void SocketServer::CloseSocket()
{
    close(m_sockfd);
    m_sockfd = -1;
}

void SocketServer::AcceptConnection()
{
    struct sockaddr_storage theirAddr = {};
    socklen_t sinSize = sizeof theirAddr;
    int newFd = accept(m_sockfd, (struct sockaddr *)&theirAddr, &sinSize);
    
    if (newFd != -1) {
        std::cout << "Accepting a new connection." << std::endl;
        std::lock_guard<std::mutex> lock(m_listenerLock);

        std::unique_ptr<SocketConnection> newConPtr = std::unique_ptr<SocketConnection>(
            new SocketConnection(
                newFd,
                std::bind(&SocketServer::ConnectionDataCallback, this, std::placeholders::_1)));
        newConPtr->SetDeletionCallback(
            std::bind(&SocketServer::RemoveConnection, this, newConPtr.get()));
        m_activeConnections.push_back(std::move(newConPtr));
    }
    else {
        std::cout << "No valid connection accepted." << std::endl;
    }
}

void SocketServer::RemoveConnection(SocketConnection* connPtr)
{
    std::cout << "Removing socket ID " << connPtr << std::endl;
    std::lock_guard<std::mutex> lock(m_listenerLock);
    m_activeConnections.erase(
        std::remove_if(
            m_activeConnections.begin(),
            m_activeConnections.end(),
            [connPtr](std::unique_ptr<SocketConnection>& item) 
            { return item.get() == connPtr; })); 
}

void SocketServer::ConnectionDataCallback(const SocketCommand&)
{

}

SocketConnection::SocketConnection(
    int sockfd,
    std::function<void(const SocketCommand&)> dataCallback)
    : m_sockfd(sockfd)
    , m_dataCallback(dataCallback)
{
    m_receiverThread = std::thread(&SocketConnection::RunReceiver, this);
}

SocketConnection::~SocketConnection()
{
    m_receiverThread.join();
}

void SocketConnection::SetDeletionCallback(std::function<void()> deletionCallback)
{
    m_deletionCallback = deletionCallback;
}

void SocketConnection::RunReceiver()
{

}