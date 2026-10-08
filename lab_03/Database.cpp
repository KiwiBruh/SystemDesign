#include "Database.h"

using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using namespace std;

/*
static string getConnectionString() {
    string host = getenv("DB_HOST");
    string port = getenv("DB_PORT");
    string user = getenv("DB_USER");
    string pass = getenv("DB_PASSWORD");
    string name = getenv("DB_NAME");

    return string("host=") + (!host.empty() ? host : "localhost") +
        " port=" + (!port.empty() ? port : "5432") +
        " user=" + (!user.empty() ? user : "postgres") +
        " password=" + (!pass.empty() ? pass : "postgres") +
        " dbname=" + (!name.empty() ? name : "lichs_db");
}
*/
static string getConnectionString() {
    const char* h = getenv("DB_HOST");
    const char* p = getenv("DB_PORT");
    const char* u = getenv("DB_USER");
    const char* pw = getenv("DB_PASSWORD");
    const char* n = getenv("DB_NAME");

    string host = h ? h : "localhost";
    string port = p ? p : "5432";
    string user = u ? u : "lichs";
    string pass = pw ? pw : "lichs_password";
    string name = n ? n : "lichs_db";

    return "host=" + host +
        " port=" + port +
        " user=" + user +
        " password=" + pass +
        " dbname=" + name;
}

void Database::init() {
    PostgreSQL::Connector::registerConnector();
    cout << "PostgreSQL connector registered" << endl;
}

Session Database::getSession() {
    return Session("PostgreSQL", getConnectionString());
}

/*
void Database::init(const string& dbPath) {

    SQLite::Connector::registerConnector();

    Session session("SQLite", dbPath);

    session << "PRAGMA foreign_keys = ON", now;

    //пользователи
    session << "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "login TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "name TEXT NOT NULL,"
        "created_at TEXT DEFAULT (datetime('now')))",
        now;

    //книги
    session << "CREATE TABLE IF NOT EXISTS books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "author TEXT NOT NULL,"
        "is_available INTEGER NOT NULL DEFAULT 1,"
        "created_at TEXT DEFAULT (datetime('now')))",
        now;

    //выдача книг
    session << "CREATE TABLE IF NOT EXISTS loans ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "book_id INTEGER NOT NULL,"
        "user_id INTEGER NOT NULL,"
        "issued_at TEXT DEFAULT (datetime('now')),"
        "due_at TEXT,"
        "returned_at TEXT,"
        "status TEXT NOT NULL DEFAULT 'active',"
        "FOREIGN KEY (book_id) REFERENCES books(id) ON DELETE CASCADE,"
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE)",
        now;
    // now это ес что от Poco keyword для того чтобы сразу выполнялось, а в дате другой now, sqlitoвский 
   
    cout << "Library database initialized: " << dbPath << endl;
}

Session Database::getSession(const string& dbPath) {
    Session session("SQLite", dbPath);
    session << "PRAGMA foreign_keys = ON", now;
    return session;
}*/