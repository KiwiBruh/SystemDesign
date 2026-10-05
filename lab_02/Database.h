#pragma once
#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>

using namespace std;

class Database {
public:
    // Инициализация: регистрирует коннектор и создаёт таблицы
    static void init(const string& dbPath);

    // Получить новую сессию для работы с БД
    static Poco::Data::Session getSession(const string& dbPath);
};