#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>  // ��� <mysql.h>, � ����������� �� ������������

#include <string>
#include <vector>
#include <iostream>

class Database {
private:
    MYSQL mysql;// ��������� ��� ������ � MySQL
    std::string host;
    std::string user;
    std::string password;
    std::string dbName;

public:
    // ����������� ���� ������
    Database(const std::string& h, const std::string& u, const std::string& p, const std::string& db);

    // ����������
    ~Database();

    // ����������� � ���� ������
    bool connect();

    // ���������� �� ���� ������
    void disconnect();

    // �������� ������ ������������
    bool createUser(const std::string& login, const std::string& password, const std::string& name);

    // �������������� ������������
    bool authenticateUser(const std::string& login, const std::string& password);

    // ���������� ��������� � ���� ������
    void saveMessage(const std::string& sender, const std::string& receiver, const std::string& message);

    // ��������� ������ ��������� ������������
    std::vector<std::string> getPersonalMessages(const std::string& user);

    // ��������� broadcast ���������
    std::vector<std::string> getBroadcastMessages();

    // ������������� ��������� ���� ������
    bool initDatabase();

private:
    // �������� ������ � ���� ������
    bool createTables();
};

#endif // DATABASE_H
