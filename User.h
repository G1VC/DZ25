#pragma once
/*Этот заголовок определяет полный интерфейс класса User с учетом:
Приватные поля:
login, password, name - основные данные пользователя
isOnline - статус онлайн
mtx - мьютекс для потокобезопасности
Конструкторы и деструктор:
По умолчанию
С параметрами
Деструктор по умолчанию
Методы доступа и модификации:
Геттеры и сеттеры для всех полей
Метод проверки пароля
Сериализация в строку
Статические методы:
Валидация полей
Проверка уникальности логина
Операторы сравнения для удобной работы с объектами
Потокобезопасность через использование мьютекса
Этот интерфейс обеспечивает полный набор функциональности 
для работы с пользовательскими данными в чат-приложении.*/
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
    std::mutex mtx;  // Для потокобезопасного доступа

public:
    // Конструкторы
    User() = default;
    User(const std::string& l, const std::string& p, const std::string& n);

    // Деструктор
    ~User() = default;

    // Методы доступа
    std::string getLogin() const;
    std::string getName() const;
    std::string getPassword() const;
    bool getOnlineStatus() const;

    // Методы установки
    void setName(const std::string& name);
    void setPassword(const std::string& password);
    void setOnlineStatus(bool status);

    // Проверка пароля
    bool checkPassword(const std::string& pass) const;

    // Сериализация в строку
    std::string toString() const;

    // Статические методы валидации
    static bool isValidLogin(const std::string& login);
    static bool isValidPassword(const std::string& password);
    static bool isValidName(const std::string& name);

private:
    // Проверка уникальности логина
    bool isLoginUnique(const std::string& login) const;
};

// Операторы сравнения
bool operator==(const User& lhs, const User& rhs);
bool operator!=(const User& lhs, const User& rhs);

#endif // USER_H

