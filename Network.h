#pragma once
/*���� ��������� ��������:
������� ����� Network ��� ������ � �������� ������������:
���������� ��������
�������� � �������������
�������� � ����� ������
��������� ������
������� NetworkUtils ���:
������������� Winsock
�������� ������
������ � IP-��������
.*/
#ifndef NETWORK_H
#define NETWORK_H
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>

// ������� ���������
const int DEFAULT_PORT = 8888;
const std::string DEFAULT_IP = "127.0.0.1";
const int BUFFER_SIZE = 1024;

class Network {
private:
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::vector<SOCKET> clientSockets;

public:
    // ������������� ������� �����������
    bool initNetwork();

    // �������� ������ �������
    bool createServerSocket(int port);

    // �������� ������ � ������ � �����
    bool bindSocket();

    // ������ ������������� �����������
    bool startListening();

    // ����� �������� �����������
    SOCKET acceptConnection();

    // �������� ������
    bool sendData(SOCKET socket, const std::string& data);

    // ����� ������
    std::string receiveData(SOCKET socket);

    // �������� ������
    void closeSocket(SOCKET socket);

    // ������� ���� ��������
    void cleanup();

private:
    // ���������� ������� ��� ��������� ������
    void handleError(const std::string& message);
};

// ������� �������� ������
class NetworkUtils {
public:
    // ������������� Winsock
    static bool initWinsock();

    // ���������� ������ � Winsock
    static void cleanupWinsock();

    // �������� ����������� �����
    static bool isPortAvailable(int port);

    // �������������� IP-������
    static in_addr getInAddr(const std::string& ip);
};

// ���������� ������� ���������
class MessageHandler {
public:
    virtual void handleMessage(SOCKET socket, const std::string& message) = 0;

    virtual void handleConnection(SOCKET socket) = 0;

    virtual void handleDisconnection(SOCKET socket) = 0;
};

#endif // NETWORK_H
