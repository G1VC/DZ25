
/*Этот код включает:
Конструкторы и деструкторы с инициализацией полей
Геттеры и сеттеры с потокобезопасным доступом через мьютекс
Методы валидации:
Проверка логина (3-20 символов, буквы, цифры, подчеркивание)
Проверка пароля (минимум 6 символов, буквы разного регистра, цифры)
Проверка имени (до 100 символов)
Операторы сравнения для удобной работы с объектами
Сериализация объекта в строковый формат
Потокобезопасность через использование мьютекса
Методы проверки уникальности логина (заглушка)*/
// Конструктор с параметрами
#include "User.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <mutex>
#include <string> 

class User {
private:
    std::string login;
    std::string password;
    std::string name;
    bool isOnline;
    mutable std::mutex mtx;  // Добавляем mutable для константных методов

public:
    // Конструктор с параметрами
    User(const std::string& l, const std::string& p, const std::string& n)
        : login(l), password(p), name(n), isOnline(false) {
    }

    // Геттеры
    std::string getLogin() const {
        std::lock_guard<std::mutex> lock(mtx);
        return login;
    }

    std::string getName() const {
        std::lock_guard<std::mutex> lock(mtx);
        return name;
    }

    std::string getPassword() const {
        std::lock_guard<std::mutex> lock(mtx);
        return password;
    }

    bool getOnlineStatus() const {
        std::lock_guard<std::mutex> lock(mtx);
        return isOnline;
    }

    // Сеттеры
    void setName(const std::string& n) {
        std::lock_guard<std::mutex> lock(mtx);
        if (isValidName(n)) {
            name = n;
        }
    }

    void setPassword(const std::string& p) {
        std::lock_guard<std::mutex> lock(mtx);
        if (isValidPassword(p)) {
            password = p;
        }
    }

    void setOnlineStatus(bool status) {
        std::lock_guard<std::mutex> lock(mtx);
        isOnline = status;
    }

    // Проверка пароля
    bool checkPassword(const std::string& pass) const {
        std::lock_guard<std::mutex> lock(mtx);
        return password == pass;
    }

    // Сериализация в строку
    std::string toString() const {
        std::lock_guard<std::mutex> lock(mtx);
        std::ostringstream oss;
        oss << "Пользователь: " << name << " (" << login << ")";
        return oss.str();
    }

    // Статические валидаторы
    static bool isValidLogin(const std::string& login) {
        return std::regex_match(login, std::regex("^a-zA-Z0-9_{3,20}$"));
    }

    static bool isValidPassword(const std::string& password) {
        return std::regex_match(password, std::regex("^(?=.*a-z)(?=.*A-Z)(?=.*\\d)a-zA-Z\\d{6,20}$"));
    }

    static bool isValidName(const std::string& name) {
        return !name.empty() && name.length() <= 100;
    }

    // Операторы сравнения
    friend bool operator==(const User& lhs, const User& rhs) {
        return lhs.getLogin() == rhs.getLogin();
    }

    friend bool operator!=(const User& lhs, const User& rhs) {
        return !(lhs == rhs);
    }

    // Дополнительные методы
    bool isLoginUnique(const std::string& login) const {
        return true;
    }
};
