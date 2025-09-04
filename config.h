/*���� ���������������� ���� ��������:
������� ���������: ���� � ���� ��� �����������
��������� ��: ��������� ��� ����������� � MySQL
������ � ��������: ������� ������� � ����� ��������
�����������: ��������� ������ �����
������������: ����������� �� ����� ��������� � ���������� �������������
���������: �������������� ��������� � ���������
����������: �������� ��� ������� �������� ������������
��� ��������� �������� � ��������� ��� �������� ��������� ������������ ��� ������ ��������� ����.*/
#pragma once
#ifndef CONFIG_H
#define CONFIG_H
#include <string>

// ������� ���������
const int DEFAULT_PORT = 8888;
const std::string DEFAULT_HOST = "127.0.0.1";

// ��������� ���� ������
const std::string DB_HOST = "localhost";
const std::string DB_USER = "root";
const std::string DB_PASSWORD = "password";
const std::string DB_NAME = "chat_db";

// ��������� �������
const int BUFFER_SIZE = 1024;

// ��������� ���������
const int RECV_TIMEOUT = 5000;  // � �������������
const int SEND_TIMEOUT = 5000;  // � �������������

// �����������
const bool ENABLE_LOGGING = true;
const std::string LOG_FILE = "chat.log";

// ��������� ������������
const int MAX_MESSAGE_LENGTH = 2048;
const int MAX_USERS = 100;

// ��������� ����������
const std::string WELCOME_MESSAGE = "����� ���������� � P2P ���!";
const std::string PROMPT = "> ";

// ��������� ���������� (��������)
const bool ENABLE_ENCRYPTION = false;

#endif // CONFIG_H
