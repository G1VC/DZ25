#include "Network.h"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h>
//using namespace NetworkUtils;
// ������������� Winsock
bool NetworkUtils::initWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "������ ������������� Winsock: " << result << std::endl;
        return false;
    }
    return true;
}

// ���������� ������ � Winsock
void NetworkUtils::cleanupWinsock() {
    WSACleanup();
}

// �������� ����������� �����
bool NetworkUtils::isPortAvailable(int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int result = bind(sock, (sockaddr*)&addr, sizeof(addr));
    closesocket(sock);
    return result == 0;
}

// �������������� IP-������
in_addr NetworkUtils::getInAddr(const std::string& ip) {
    in_addr addr;
    inet_pton(AF_INET, ip.c_str(), &addr);
    return addr;
}

// ������������� ������� �����������
bool Network::initNetwork() {
    return NetworkUtils::initWinsock() && createServerSocket(DEFAULT_PORT);
}

// �������� ������ �������
bool Network::createServerSocket(int port) {
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        handleError("������ �������� ������");
        return false;
    }
    return true;
}

// �������� ������ � ������ � �����
bool Network::bindSocket() {
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        handleError("������ �������� ������");
        return false;
    }
    return true;
}

// ������ ������������� �����������
bool Network::startListening() {
    if (!bindSocket()) {
        return false;
    }
    return true;
}

// ����� �������� �����������
SOCKET Network::acceptConnection() {
    SOCKET newSocket = accept(serverSocket, NULL, NULL);
    if (newSocket == INVALID_SOCKET) {
        handleError("������ �������� �����������");
    }
    return newSocket;
}

// �������� ������
bool Network::sendData(SOCKET socket, const std::string& data) {
    int result = send(socket, data.c_str(), data.length(), 0);
    if (result == SOCKET_ERROR) {
        handleError("������ �������� ������");
        return false;
    }
    return true;
}

// ����� ������
std::string Network::receiveData(SOCKET socket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived == SOCKET_ERROR) {
        handleError("������ ������ ������");
        return "";
    }
    buffer[bytesReceived] = '\0';
    return std::string(buffer);
}

// �������� ������
void Network::closeSocket(SOCKET socket) {
    closesocket(socket);
}

// ������� ���� ��������
void Network::cleanup() {
    for (SOCKET socket : clientSockets) {
        closeSocket(socket);
    }
    closeSocket(serverSocket);
    NetworkUtils::cleanupWinsock();
}

// ��������� ������
void Network::handleError(const std::string& message) {
    std::cerr << "������� ������: " << message << std::endl;
    std::cerr << "��� ������: " << WSAGetLastError() << std::endl;
}

