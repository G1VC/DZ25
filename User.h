#pragma once
/*���� ��������� ���������� ������ ��������� ������ User � ������:
��������� ����:
login, password, name - �������� ������ ������������
isOnline - ������ ������
mtx - ������� ��� ������������������
������������ � ����������:
�� ���������
� �����������
���������� �� ���������
������ ������� � �����������:
������� � ������� ��� ���� �����
����� �������� ������
������������ � ������
����������� ������:
��������� �����
�������� ������������ ������
��������� ��������� ��� ������� ������ � ���������
������������������ ����� ������������� ��������
���� ��������� ������������ ������ ����� ���������������� 
��� ������ � ����������������� ������� � ���-����������.*/
#ifndef USER_H
#define USER_H

#include <string>
#include <mutex>

class User {
private:
    std::string login;
    std::string password;
    std::string name;
    bool isOnline;
    std::mutex mtx;  // ��� ����������������� �������

public:
    // ������������
    User() = default;
    User(const std::string& l, const std::string& p, const std::string& n);

    // ����������
    ~User() = default;

    // ������ �������
    std::string getLogin() const;
    std::string getName() const;
    std::string getPassword() const;
    bool getOnlineStatus() const;

    // ������ ���������
    void setName(const std::string& name);
    void setPassword(const std::string& password);
    void setOnlineStatus(bool status);

    // �������� ������
    bool checkPassword(const std::string& pass) const;

    // ������������ � ������
    std::string toString() const;

    // ����������� ������ ���������
    static bool isValidLogin(const std::string& login);
    static bool isValidPassword(const std::string& password);
    static bool isValidName(const std::string& name);

private:
    // �������� ������������ ������
    bool isLoginUnique(const std::string& login) const;
};

// ��������� ���������
bool operator==(const User& lhs, const User& rhs);
bool operator!=(const User& lhs, const User& rhs);

#endif // USER_H

