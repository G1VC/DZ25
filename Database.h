#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>  // или <mysql.h>, в зависимости от конфигурации

#include <string>
#include <vector>
#include <iostream>

class Database {
private:
    MYSQL mysql;// Структура для работы с MySQL
    std::string host;
    std::string user;
    std::string password;
    std::string dbName;

public:
    // Конструктор базы данных
    Database(const std::string& h, const std::string& u, const std::string& p, const std::string& db);

    // Деструктор
    ~Database();

    // Подключение к базе данных
    bool connect();

    // Отключение от базы данных
    void disconnect();

    // Создание нового пользователя
    bool createUser(const std::string& login, const std::string& password, const std::string& name);

    // Аутентификация пользователя
    bool authenticateUser(const std::string& login, const std::string& password);

    // Сохранение сообщения в базе данных
    void saveMessage(const std::string& sender, const std::string& receiver, const std::string& message);

    // Получение личных сообщений пользователя
    std::vector<std::string> getPersonalMessages(const std::string& user);

    // Получение broadcast сообщений
    std::vector<std::string> getBroadcastMessages();

    // Инициализация структуры базы данных
    bool initDatabase();

private:
    // Создание таблиц в базе данных
    bool createTables();
};

#endif // DATABASE_H
