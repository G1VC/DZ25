/*Этот конфигурационный файл содержит:
Сетевые параметры: порт и хост для подключения
Параметры БД: настройки для подключения к MySQL
Буферы и таймауты: размеры буферов и время ожидания
Логирование: настройки записи логов
Безопасность: ограничения на длину сообщений и количество пользователей
Интерфейс: приветственные сообщения и подсказки
Шифрование: заглушка для будущих настроек безопасности
Все параметры вынесены в константы для удобства изменения конфигурации без правки основного кода.*/
#pragma once
#ifndef CONFIG_H
#define CONFIG_H
#include <string>

// Сетевые настройки
const int DEFAULT_PORT = 8888;
const std::string DEFAULT_HOST = "127.0.0.1";

// Настройки базы данных
const std::string DB_HOST = "localhost";
const std::string DB_USER = "root";
const std::string DB_PASSWORD = "password";
const std::string DB_NAME = "chat_db";

// Настройки буферов
const int BUFFER_SIZE = 1024;

// Настройки таймаутов
const int RECV_TIMEOUT = 5000;  // в миллисекундах
const int SEND_TIMEOUT = 5000;  // в миллисекундах

// Логирование
const bool ENABLE_LOGGING = true;
const std::string LOG_FILE = "chat.log";

// Настройки безопасности
const int MAX_MESSAGE_LENGTH = 2048;
const int MAX_USERS = 100;

// Настройки интерфейса
const std::string WELCOME_MESSAGE = "Добро пожаловать в P2P чат!";
const std::string PROMPT = "> ";

// Настройки шифрования (заглушка)
const bool ENABLE_ENCRYPTION = false;

#endif // CONFIG_H
