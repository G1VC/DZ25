// main.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
/**/
#include "Chat.h"
#include "Database.h"
#include "Network.h"
#include <iostream>
#include <string>
#include <exception>
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

// Функция-обёртка для потока
static DWORD WINAPI processMessagesWrapper(LPVOID chatPtr) {
    Chat* chatInstance = static_cast<Chat*>(chatPtr);
    chatInstance->processMessages();
    return 0;
}

// Функция для обрезки пробелов в начале и конце строки
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

int main() {
    setlocale(LC_ALL, "");
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Ошибка инициализации Winsock" << std::endl;
        return 1;
    }

    // Инициализация базы данных
    Database db("localhost", "root", "password", "chat_db");
    if (!db.connect()) {
        std::cout << "Ошибка подключения к базе данных" << std::endl;
        WSACleanup();
        return 1;
    }

    Chat chat(db);
    HANDLE messageProcessor = CreateThread(
        NULL,
        0,
        processMessagesWrapper,
        &chat,
        0,
        NULL
    );

    if (messageProcessor == NULL) {
        std::cout << "Ошибка создания потока обработки сообщений" << std::endl;
        WSACleanup();
        return 1;
    }

    std::string command;
    std::string login, password, name;
    bool isLoggedIn = false;
    std::string currentUser = "";

    std::cout << "Добро пожаловать в P2P чат!" << std::endl;
    std::cout << "Доступные команды:\n"
        << "1 - регистрация. Используйте формат: 1 <login> <password> <name>\n"
        << "2 - вход в систему. Используйте формат: 2 <login> <password>\n"
        << "7 - выход из чата\n" << std::endl;

    std::cout << "ПОСЛЕ регистрации и входа в систему доступны команды:\n"
        << "3 - отправка личного сообщения. Используйте формат: 3 <receiver> <message>\n"
        << "4 - отправка сообщения всем. Используйте формат: 4 <message>\n"
        << "5 - показать онлайн пользователей\n"
        << "6 - выход из системы\n"
        << "7 - выход из чата\n"
        << "8 - просмотр истории личных сообщений\n"
        << "9 - просмотр истории общих сообщений\n" << std::endl;

    try {
        while (true) {
            std::cout << (isLoggedIn ? currentUser : ">") << ": ";
            std::getline(std::cin, command);
            if (command.empty()) continue;

            // Разделяем команду и аргументы
            char cmd = command[0];
            std::string args = trim(command.substr(1));

            if (cmd == '1') { // REGISTER
                if (isLoggedIn) {
                    std::cout << "Сначала выйдите из системы!" << std::endl;
                    continue;
                }

                size_t pos1 = args.find(' ');
                size_t pos2 = args.find(' ', pos1 + 1);
                if (pos1 == std::string::npos || pos2 == std::string::npos) {
                    std::cout << "Неверный формат команды! Используйте: 1 <login> <password> <name>" << std::endl;
                    continue;
                }

                // Продолжение обработки команд

                login = trim(args.substr(0, pos1));
                password = trim(args.substr(pos1 + 1, pos2 - pos1 - 1));
                name = trim(args.substr(pos2 + 1));

                // Проверяем, что все поля заполнены
                if (login.empty() || password.empty() || name.empty()) {
                    std::cout << "Все поля должны быть заполнены!" << std::endl;
                    continue;
                }

                // Проверяем минимальную длину полей
                if (login.length() < 3 || password.length() < 3 || name.length() < 3) {
                    std::cout << "Все поля должны содержать минимум 3 символа!" << std::endl;
                    continue;
                }

                if (chat.userExists(login)) {
                    std::cout << "Пользователь с таким логином уже существует!" << std::endl;
                    continue;
                }

                chat.registerUser(login, password, name);

            }
            else if (cmd == '2' && !isLoggedIn) { // LOGIN
                size_t pos1 = args.find(' ');
                if (pos1 == std::string::npos) {
                    std::cout << "Неверный формат команды! Используйте формат: 2 <login> <password>" << std::endl;
                    continue;
                }

                login = trim(args.substr(0, pos1));
                password = trim(args.substr(pos1 + 1));

                if (!chat.userExists(login)) {
                    std::cout << "Пользователь не найден!" << std::endl;
                    continue;
                }

                if (chat.loginUser(login, password)) {
                    isLoggedIn = true;
                    currentUser = login;
                    std::cout << "Успешный вход в систему!" << std::endl;
                }
                else {
                    std::cout << "Ошибка входа! Неверный пароль." << std::endl;
                }
            }
            else if (cmd == '3' && isLoggedIn) { // SEND
                size_t pos1 = args.find(' ');
                if (pos1 == std::string::npos) {
                    std::cout << "Неверный формат команды! Используйте: 3 <receiver> <message>" << std::endl;
                    continue;
                }

                std::string receiver = trim(args.substr(0, pos1));
                std::string message = trim(args.substr(pos1 + 1));

                if (receiver.empty() || message.empty()) {
                    std::cout << "Необходимо указать получателя и текст сообщения" << std::endl;
                    continue;
                }

                if (!chat.userExists(receiver)) {
                    std::cout << "Пользователь " << receiver << " не найден" << std::endl;
                    continue;
                }

                chat.sendMessage(currentUser, receiver, message);
            }
            else if (cmd == '4' && isLoggedIn) { // BROADCAST
                if (args.empty()) {
                    std::cout << "Введите текст сообщения для рассылки" << std::endl;
                    continue;
                }
                chat.broadcastMessage(currentUser, trim(args));
            }
            else if (cmd == '5' && isLoggedIn) { // LIST
                std::vector<User> onlineUsers = chat.getOnlineUsers();
                std::cout << "Пользователи онлайн:" << std::endl;
                if (onlineUsers.empty()) {
                    std::cout << "Никто не онлайн" << std::endl;
                }
                else {
                    for (const auto& user : onlineUsers) {
                        std::cout << "- " << user.getName() << " (" << user.getLogin() << ")" << std::endl;
                    }
                }
            }
            else if (cmd == '6' && isLoggedIn) { // LOGOUT
                chat.logoutUser(currentUser);
                isLoggedIn = false;
                currentUser = "";
                std::cout << "Вы вышли из системы" << std::endl;
            }
            else if (cmd == '7') { // EXIT
                std::cout << "До свидания!" << std::endl;
                break;
            }
            else if (cmd == '8' && isLoggedIn) { // История личных сообщений
                std::vector<Message> personalMessages = chat.getPersonalMessages(currentUser);
                // История личных сообщений
                std::cout << "История личных сообщений для " << currentUser << ":\n";
                for (const auto& msg : personalMessages) {
                    std::cout << msg.sender << " -> " << msg.receiver << ": " << msg.text << std::endl;
                }

}
            else if (cmd == '9' && isLoggedIn) { // История broadcast сообщений
                std::vector<Message> broadcastMessages = chat.getBroadcastMessages();
                std::cout << "История общих сообщений:\n";
                for (const auto& msg : broadcastMessages) {
                    std::cout << msg.sender << ": " << msg.text << std::endl;
                }

}
            else {
                std::cout << "!!! Неизвестная команда. Возможно Вы не вошли в систему!:\n"
                    << "Доступные команды:\n"
                    << "1 - регистрация. Используйте формат: 1 <login> <password> <name>\n"
                    << "2 - вход в систему. Используйте формат: 2 <login> <password>\n"
                    << "7 - выход из чата\n"
                    << "ПОСЛЕ регистрации и входа в систему доступны команды:\n" << std::endl;
                std::cout << "3 - отправка личного сообщения. Используйте формат: 3 <receiver> <message>\n"
                    << "4 - отправка сообщения всем. Используйте формат: 4 <message>\n"
                    << "5 - показать онлайн пользователей\n"
                    << "6 - выход из системы\n"
                    << "7 - выход из чата\n"
                    << "8 - просмотр истории личных сообщений\n"
                    << "9 - просмотр истории общих сообщений\n" << std::endl;
                    }

} // конец цикла while

}
catch (const std::exception& e) {
    std::cerr << "Произошла ошибка: " << e.what() << std::endl;
}

// Завершаем поток обработки сообщений
if (messageProcessor != NULL) {
    WaitForSingleObject(messageProcessor, INFINITE);
    CloseHandle(messageProcessor);
}

WSACleanup();
return 0;

}
