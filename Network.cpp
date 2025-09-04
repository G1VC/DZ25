#include "Network.h"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h>
//using namespace NetworkUtils;
// Инициализация Winsock
bool NetworkUtils::initWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "Ошибка инициализации Winsock: " << result << std::endl;
        return false;
    }
    return true;
}

// Завершение работы с Winsock
void NetworkUtils::cleanupWinsock() {
    WSACleanup();
}

// Проверка доступности порта
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

// Преобразование IP-адреса
in_addr NetworkUtils::getInAddr(const std::string& ip) {
    in_addr addr;
    inet_pton(AF_INET, ip.c_str(), &addr);
    return addr;
}

// Инициализация сетевых компонентов
bool Network::initNetwork() {
    return NetworkUtils::initWinsock() && createServerSocket(DEFAULT_PORT);
}

// Создание сокета сервера
bool Network::createServerSocket(int port) {
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        handleError("Ошибка создания сокета");
        return false;
    }
    return true;
}

// Привязка сокета к адресу и порту
bool Network::bindSocket() {
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        handleError("Ошибка привязки сокета");
        return false;
    }
    return true;
}

// Начало прослушивания подключений
bool Network::startListening() {
    if (!bindSocket()) {
        return false;
    }
    return true;
}

// Прием входящих подключений
SOCKET Network::acceptConnection() {
    SOCKET newSocket = accept(serverSocket, NULL, NULL);
    if (newSocket == INVALID_SOCKET) {
        handleError("Ошибка принятия подключения");
    }
    return newSocket;
}

// Отправка данных
bool Network::sendData(SOCKET socket, const std::string& data) {
    int result = send(socket, data.c_str(), data.length(), 0);
    if (result == SOCKET_ERROR) {
        handleError("Ошибка отправки данных");
        return false;
    }
    return true;
}

// Прием данных
std::string Network::receiveData(SOCKET socket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived == SOCKET_ERROR) {
        handleError("Ошибка приема данных");
        return "";
    }
    buffer[bytesReceived] = '\0';
    return std::string(buffer);
}

// Закрытие сокета
void Network::closeSocket(SOCKET socket) {
    closesocket(socket);
}

// Очистка всех ресурсов
void Network::cleanup() {
    for (SOCKET socket : clientSockets) {
        closeSocket(socket);
    }
    closeSocket(serverSocket);
    NetworkUtils::cleanupWinsock();
}

// Обработка ошибок
void Network::handleError(const std::string& message) {
    std::cerr << "Сетевая ошибка: " << message << std::endl;
    std::cerr << "Код ошибки: " << WSAGetLastError() << std::endl;
}

