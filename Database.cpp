#include "Database.h"
#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>  

class Database {
private:
    MYSQL mysql; 
    std::string host;
    std::string user;
    std::string password;
    std::string dbName;

public:
    Database(const std::string& h, const std::string& u, const std::string& p, const std::string& db);
    ~Database();

    bool connect();
    void disconnect();
    bool createUser(const std::string& login, const std::string& password, const std::string& name);
    bool authenticateUser(const std::string& login, const std::string& password);
    void saveMessage(const std::string& sender, const std::string& receiver, const std::string& message);
    std::vector<std::string> getPersonalMessages(const std::string& user);
    std::vector<std::string> getBroadcastMessages();
    bool initDatabase();
    bool setUserStatus(const std::string& login, bool status);
    bool getUserStatus(const std::string& login);
    std::vector<std::string> getOnlineUsers();
};

Database::Database(const std::string& h, const std::string& u, const std::string& p, const std::string& db)
    : host(h), user(u), password(p), dbName(db) {
    mysql_init(&mysql);  // Теперь это будет работать
}

Database::~Database() {
    mysql_close(&mysql);  // И здесь тоже
}

bool Database::connect() {
    if (mysql_real_connect(&mysql, host.c_str(), user.c_str(), password.c_str(), dbName.c_str(), 0, NULL, 0) == NULL) {
        std::cerr << "Ошибка подключения к БД: " << mysql_error(&mysql) << std::endl;
        return false;
    }
    return true;
}

void Database::disconnect() {
    mysql_close(&mysql);
}

bool Database::createUser(const std::string& login, const std::string& password, const std::string& name) {
    std::string query = "INSERT INTO users (login, password, name) VALUES ('" + login + "', '" + password + "', '" + name + "')";
    if (mysql_query(&mysql, query.c_str())) {
        std::cerr << "Ошибка создания пользователя: " << mysql_error(&mysql) << std::endl;
        return false;
    }
    return true;
}

bool Database::authenticateUser(const std::string& login, const std::string& password) {
    std::string query = "SELECT * FROM users WHERE login = '" + login + "' AND password = '" + password + "'";
    if (mysql_query(&mysql, query.c_str())) {
        std::cerr << "Ошибка аутентификации: " << mysql_error(&mysql) << std::endl;
        return false;
    }
    MYSQL_RES* result = mysql_store_result(&mysql);
    return mysql_num_rows(result) > 0;
}

void Database::saveMessage(const std::string& sender, const std::string& receiver, const std::string& message) {
    std::string query = "INSERT INTO messages (sender, receiver, message) VALUES ('" + sender + "', '" + receiver + "', '" + message + "')";
    mysql_query(&mysql, query.c_str());
}

std::vector<std::string> Database::getPersonalMessages(const std::string& user) {
    std::vector<std::string> messages;
    std::string query = "SELECT message FROM messages WHERE receiver = '" + user + "'";
    if (mysql_query(&mysql, query.c_str())) {
        std::cerr << "Ошибка получения сообщений: " << mysql_error(&mysql) << std::endl;
        return messages;
    }
    MYSQL_RES* result = mysql_store_result(&mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        messages.push_back(row[0]);
    }
    mysql_free_result(result);
    return messages;
}

std::vector<std::string> Database::getBroadcastMessages() {
    std::vector<std::string> messages;
    std::string query = "SELECT message FROM messages WHERE receiver = 'ALL'";
    if (mysql_query(&mysql, query.c_str())) {
        std::cerr << "Ошибка получения broadcast сообщений: " << mysql_error(&mysql) << std::endl;
        return messages;
    }
    MYSQL_RES* result = mysql_store_result(&mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        messages.push_back(row[0]);
    }
    mysql_free_result(result);
    return messages;
}

bool Database::initDatabase() {
    // Создание таблицы пользователей
    std::string createUsersTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "login VARCHAR(50) UNIQUE NOT NULL,"
        "password VARCHAR(255) NOT NULL,"
        "name VARCHAR(100) NOT NULL"
        ");";

    // Создание таблицы сообщений
    std::string createMessagesTable =
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "sender VARCHAR(50) NOT NULL,"
        "receiver VARCHAR(50) NOT NULL,"
        "message TEXT NOT NULL,"
        "FOREIGN KEY (sender) REFERENCES users(login)"
        ");";

    // Создание таблицы онлайн-статуса
    std::string createStatusTable =
        "CREATE TABLE IF NOT EXISTS status ("
        "user_id INT PRIMARY KEY,"
        "is_online BOOLEAN DEFAULT FALSE,"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ");";

    // Выполняем создание таблиц
    if (mysql_query(&mysql, createUsersTable.c_str())) {
        std::cerr << "Ошибка создания таблицы users: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    if (mysql_query(&mysql, createMessagesTable.c_str())) {
        std::cerr << "Ошибка создания таблицы messages: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    if (mysql_query(&mysql, createStatusTable.c_str())) {
        std::cerr << "Ошибка создания таблицы status: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    return true;
}

// Дополнительные методы для работы со статусом
bool Database::setUserStatus(const std::string& login, bool status) {
    std::string query = "UPDATE users SET is_online = " + (status ? "TRUE" : "FALSE") +
        " WHERE login = '" + login + "'";
    if (mysql_query(&mysql, query.c_str())) {
        std::cerr << "Ошибка обновления статуса пользователя: " << mysql_error(&mysql) << std::endl;
        return false;
    }
    return true;
}

bool Database::getUserStatus(const std::string& login) {
    std::string query = "SELECT is_online FROM users WHERE login = '" + login + "'";
    if (mysql_query(&mysql, query.c_str())) {
        std::cerr << "Ошибка получения статуса пользователя: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    MYSQL_ROW row = mysql_fetch_row(result);
    mysql_free_result(result);

    return row ? (row[0] == "1") : false;
}

// Метод для получения списка онлайн-пользователей
std::vector<std::string> Database::getOnlineUsers() {
    std::vector<std::string> onlineUsers;
    std::string query = "SELECT login FROM users WHERE is_online = TRUE";

    if (mysql_query(&mysql, query.c_str())) {
        std::cerr << "Ошибка получения списка онлайн-пользователей: " << mysql_error(&mysql) << std::endl;
        return onlineUsers;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        onlineUsers.push_back(row[0]);
    }

    mysql_free_result(result);
    return onlineUsers;
}
