#include "Chat.h"
#include "Database.h"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <WinSock2.h>

// Реализация класса User
User::User(std::string l, std::string p, std::string n)
    : login(l), password(p), name(n), isOnline(false) {
}

bool User::checkPassword(const std::string& pass) { return password == pass; }
void User::setOnline(bool status) { isOnline = status; }
std::string User::getName() const { return name; }
std::string User::getLogin() const { return login; }
bool User::getOnline() const { return isOnline; }

// Реализация класса Message
Message::Message(std::string s, std::string r, std::string t)
    : sender(s), receiver(r), text(t) {
}

// Реализация класса Chat с интеграцией MySQL
Chat::Chat(Database& database) : db(database) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock" << std::endl;
        return;
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки сокета: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
}

Chat::~Chat() {
    for (auto& socket : userSockets) {
        closesocket(socket.second);
    }
    closesocket(serverSocket);
    WSACleanup();
}

void Chat::registerUser(std::string login, std::string password, std::string name) {
    if (db.createUser(login, password, name)) {
        std::cout << "Пользователь " << login << " зарегистрирован" << std::endl;
    }
    else {
        std::cout << "Ошибка при регистрации пользователя" << std::endl;
    }
}

bool Chat::loginUser(std::string login, std::string password) {
    return db.authenticateUser(login, password);
}

void Chat::sendMessage(std::string sender, std::string receiver, std::string text) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        db.saveMessage(sender, receiver, text);
    }

    auto it = userSockets.find(receiver);
    if (it != userSockets.end()) {
        std::string msg = sender + ":" + receiver + ":" + text;
        sendto(
            serverSocket,
            msg.c_str(),
            msg.length(),
            0,
            (sockaddr*)&serverAddr,
            sizeof(serverAddr)
        );
    }
}

void Chat::broadcastMessage(std::string sender, std::string text) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        db.saveMessage(sender, "ALL", text);
    }

    // Получаем список онлайн пользователей из БД
    auto onlineUsers = getOnlineUsers();
    for (const auto& user : onlineUsers) {
        if (user.getLogin() != sender) {
            sendMessage(sender, user.getLogin(), text);
        }
    }
}

void Chat::processMessages() {
    char buffer[1024];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    while (true) {
        int bytesReceived = recvfrom(
            serverSocket,
            buffer,
            sizeof(buffer),
            0,
            (sockaddr*)&senderAddr,
            &senderAddrSize
        );

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string message(buffer);
            size_t colon1 = message.find(':');
            size_t colon2 = message.find(':', colon1 + 1);

            if (colon1 != std::string::npos && colon2 != std::string::npos) {
                std::string sender = message.substr(0, colon1);
                std::string receiver = message.substr(colon1 + 1, colon2 - colon1 - 1);
                std::string text = message.substr(colon2 + 1);

                if (receiver == "ALL") {
                    // Сохраняем в БД и рассылаем всем
                    db.saveMessage(sender, "ALL", text);
                    broadcastMessage(sender, text);
                }
                else {
                    // Сохраняем в БД и отправляем получателю
                    db.saveMessage(sender, receiver, text);
                    sendMessage(sender, receiver, text);
                }

                // Логируем полученное сообщение
                std::cout << sender << " -> " << receiver << ": " << text << std::endl;
            }
        }
    }
}

vector<Message> Chat::getPersonalMessages(const string& user) {
    vector<Message> result;
    vector<string> messages = db.getPersonalMessages(user);

    for (const auto& msg : messages) {
        result.emplace_back(Message(user, user, msg));
    }

    return result;
}

vector<Message> Chat::getBroadcastMessages() {
    vector<Message> result;
    vector<string> messages = db.getBroadcastMessages();

    for (const auto& msg : messages) {
        result.emplace_back(Message("ALL", "ALL", msg));
    }

    return result;
}
