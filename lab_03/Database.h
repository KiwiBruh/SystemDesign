#pragma once
#include <Poco/Data/Session.h>
//#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/PostgreSQL/Connector.h>

using namespace std;

class Database {
public:
    /*
    //это для sqlite: надо передавать базу в pg ниче не надо
    // Инициализация: регистрирует коннектор и создаёт таблицы
    static void init(const string& dbPath);

    // Получить новую сессию для работы с БД
    static Poco::Data::Session getSession(const string& dbPath);
    */
    // регистрирует коннектор
    static void init();

    // сессия PostgreSQL
    static Poco::Data::Session getSession();
};