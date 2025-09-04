#pragma once

#ifndef CHAT_H
#define CHAT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <WinSock2.h>  // Для работы с сокетами
#include <windows.h>
#include "Database.h"  // Подключение модуля работы с БД

using namespace std;

// Класс для хранения информации о пользователе
class User {
private:
    string login;
    string password;
    string name;
    bool isOnline;

public:
    User(string l, string p, string n);
    bool checkPassword(const string& pass);
    void setOnline(bool status);
    string getName() const;
    string getLogin() const;
    bool getOnline() const;
};

// Класс для хранения сообщений
class Message {
public:
    string sender;
    string receiver;
    string text;
    Message(string s, string r, string t);
};

class Chat {
private:
    Database db;  // Объект для работы с БД
    unordered_map<string, int> userSockets;
    mutex mtx;
    SOCKET serverSocket;
    sockaddr_in serverAddr;

public:
    Chat(Database& database);  // Конструктор с передачей БД
    ~Chat();

    void registerUser(string login, string password, string name);
    bool loginUser(string login, string password);
    void sendMessage(string sender, string receiver, string text);
    void broadcastMessage(string sender, string text);
    void processMessages();
    vector<Message> getPersonalMessages(const string& user);
    vector<Message> getBroadcastMessages();
    bool userExists(const string& login) const;
    vector<User> getOnlineUsers() const;
    void logoutUser(const string& login);
};

#endif
