#pragma once
/*Этот заголовок включает:
Базовый класс Network для работы с сетевыми соединениями:
Управление сокетами
Привязка и прослушивание
Отправка и прием данных
Обработка ошибок
Утилиты NetworkUtils для:
Инициализации Winsock
Проверки портов
Работы с IP-адресами
.*/
#ifndef NETWORK_H
#define NETWORK_H
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>

// Сетевые константы
const int DEFAULT_PORT = 8888;
const std::string DEFAULT_IP = "127.0.0.1";
const int BUFFER_SIZE = 1024;

class Network {
private:
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::vector<SOCKET> clientSockets;

public:
    // Инициализация сетевых компонентов
    bool initNetwork();

    // Создание сокета сервера
    bool createServerSocket(int port);

    // Привязка сокета к адресу и порту
    bool bindSocket();

    // Начало прослушивания подключений
    bool startListening();

    // Прием входящих подключений
    SOCKET acceptConnection();

    // Отправка данных
    bool sendData(SOCKET socket, const std::string& data);

    // Прием данных
    std::string receiveData(SOCKET socket);

    // Закрытие сокета
    void closeSocket(SOCKET socket);

    // Очистка всех ресурсов
    void cleanup();

private:
    // Внутренняя функция для обработки ошибок
    void handleError(const std::string& message);
};

// Утилиты сетевого уровня
class NetworkUtils {
public:
    // Инициализация Winsock
    static bool initWinsock();

    // Завершение работы с Winsock
    static void cleanupWinsock();

    // Проверка доступности порта
    static bool isPortAvailable(int port);

    // Преобразование IP-адреса
    static in_addr getInAddr(const std::string& ip);
};

// Обработчик сетевых сообщений
class MessageHandler {
public:
    virtual void handleMessage(SOCKET socket, const std::string& message) = 0;

    virtual void handleConnection(SOCKET socket) = 0;

    virtual void handleDisconnection(SOCKET socket) = 0;
};

#endif // NETWORK_H
