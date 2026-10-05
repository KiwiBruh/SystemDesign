#include "Handlers.h"
#include <Poco/JSON/Object.h>

using namespace Poco::Net;
using namespace Poco::Data::Keywords;
using namespace std;

inline string readBody(HTTPServerRequest& request) { //штука чтобы тело как строку читать
    ostringstream ss;
    ss << request.stream().rdbuf();
    return ss.str();
}

void PingHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
        response.setStatus(HTTPResponse::HTTP_OK);
        response.setContentType("application/json");
        ostream& out = response.send();
        Poco::JSON::Object obj;
        obj.set("status", "ok");
        obj.stringify(out);
}

void RegisterHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {

    // читаем парсим JSON
    Poco::JSON::Object::Ptr json;
    try {
        Poco::JSON::Parser parser;
        json = parser.parse(readBody(request)).extract<Poco::JSON::Object::Ptr>();
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Invalid JSON");
        err.stringify(response.send());
        return;
    }

    // берем поля
    string login, password, name;
    try {
        login = json->getValue<string>("login");
        password = json->getValue<string>("password");
        name = json->getValue<string>("name");
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Fields 'login', 'password', 'name' are required");
        err.stringify(response.send());
        return;
    }

    // сверяем требования к полям
    if (login.size() < 3 || password.size() < 6 || name.empty()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "login >= 3 chars, password >= 6 chars, name not empty");
        err.stringify(response.send());
        return;
    }

    auto session = Database::getSession("Lichs.db");
    int count = 0;
    session << "SELECT COUNT(*) FROM users WHERE login = ?", // это если вдруг такой пользователь уже существует
        use(login), into(count), now;

    if (count > 0) {
        response.setStatus(HTTPResponse::HTTP_CONFLICT);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Login already taken");
        err.stringify(response.send());
        return;
    }

    //если все проверки пройдены то регаем пользователя
    string pwdHash = hashPassword(password);
    session << "INSERT INTO users (login, password_hash, name) VALUES (?, ?, ?)",
        use(login), use(pwdHash), use(name), now;

    // если все получилось то выведем добрый респонс с данными
    Poco::Int64 newId = 0;
    session << "SELECT last_insert_rowid()", into(newId), now;
    response.setStatus(HTTPResponse::HTTP_CREATED);
    response.setContentType("application/json");
    Poco::JSON::Object result;
    result.set("id", static_cast<int>(newId));
    result.set("login", login);
    result.set("name", name);
    result.stringify(response.send());
}

void LoginHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {

    Poco::JSON::Object::Ptr json;
    try {
        Poco::JSON::Parser parser;
        json = parser.parse(readBody(request)).extract<Poco::JSON::Object::Ptr>();
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Invalid JSON");
        err.stringify(response.send());
        return;
    }

    string login, password;
    try {
        login = json->getValue<string>("login");
        password = json->getValue<string>("password");
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Fields 'login' and 'password' are required");
        err.stringify(response.send());
        return;
    }

    // ищем пользователя в БД
    auto session = Database::getSession("Lichs.db");
    int userId = 0;
    string storedHash;

    session << "SELECT id, password_hash FROM users WHERE login = ?",
        use(login), into(userId), into(storedHash), now;

    // прроверка входных данных
    if (userId == 0 || storedHash != hashPassword(password)) {
        response.setStatus(HTTPResponse::HTTP_UNAUTHORIZED);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Invalid credentials");
        err.stringify(response.send());
        return;
    }

    // создаем  JWT
    string token = generateJwt(userId, login);
    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    Poco::JSON::Object result;
    result.set("token", token);
    result.set("login", login);
    result.stringify(response.send());
}

void BookCreateHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    using namespace Poco::Data::Keywords;

    // проверка JWT
    auto auth = checkAuth(request, response);
    if (!auth.ok) return;


    Poco::JSON::Object::Ptr json;
    try {
        Poco::JSON::Parser parser;
        json = parser.parse(readBody(request)).extract<Poco::JSON::Object::Ptr>();
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Invalid JSON");
        err.stringify(response.send());
        return;
    }

    string title, author;
    try {
        title = json->getValue<string>("title");
        author = json->getValue<string>("author");
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Fields 'title' and 'author' are required");
        err.stringify(response.send());
        return;
    }


    if (title.empty() || author.empty()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "title and author must not be empty");
        err.stringify(response.send());
        return;
    }

    // запись книги в бд
    auto session = Database::getSession("Lichs.db");
    session << "INSERT INTO books (title, author, is_available) VALUES (?, ?, 1)",
        use(title), use(author), now;


    Poco::Int64 newId = 0;
    session << "SELECT last_insert_rowid()", into(newId), now;
    response.setStatus(HTTPResponse::HTTP_CREATED);
    response.setContentType("application/json");
    Poco::JSON::Object result;
    result.set("id", static_cast<int>(newId));
    result.set("title", title);
    result.set("author", author);
    result.set("is_available", true);
    result.set("added_by", auth.login);  // кто добавил
    result.stringify(response.send());
}

/*
void LoanCreateHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {

    // проверка JWT, user_id  кстати берём из токена, а не из тела!!!!!!!!
    auto auth = checkAuth(request, response);
    if (!auth.ok) return;

    Poco::JSON::Object::Ptr json;
    try {
        Poco::JSON::Parser parser;
        json = parser.parse(readBody(request)).extract<Poco::JSON::Object::Ptr>();
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Invalid JSON");
        err.stringify(response.send());
        return;
    }


    int bookId = 0;
    try {
        bookId = json->getValue<int>("book_id");
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Field 'book_id' is required");
        err.stringify(response.send());
        return;
    }

    // поскольку здесь несколько запросов, то сделаем их одной транзакцией
    auto session = Database::getSession("Lichs.db");
    session.begin();

    try {
        // книга существует и доступна
        int available = -1;  // -1 = "не найдена"
        session << "SELECT is_available FROM books WHERE id = ?",
            use(bookId), into(available), now;

        if (available == -1) {
            session.rollback();// откат транзакции
            response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Book not found");
            err.stringify(response.send());
            return;
        }

        if (available == 0) {
            session.rollback();
            response.setStatus(HTTPResponse::HTTP_CONFLICT);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Book is already issued");
            err.stringify(response.send());
            return;
        }

        // отметили что выдали книгу
        session << "UPDATE books SET is_available = 0 WHERE id = ?",
            use(bookId), now;

        // создаём книжный должок 
        session << "INSERT INTO loans (book_id, user_id, status, due_at) "
            "VALUES (?, ?, 'active', datetime('now', '+30 days'))",
            use(bookId), use(auth.userId), now;

        Poco::Int64 loanId = 0;
        session << "SELECT last_insert_rowid()", into(loanId), now;

        // коммит транзакции
        session.commit();

        response.setStatus(HTTPResponse::HTTP_CREATED);
        response.setContentType("application/json");
        Poco::JSON::Object result;
        result.set("id", static_cast<int>(loanId));
        result.set("book_id", bookId);
        result.set("user_id", auth.userId);
        result.set("login", auth.login);
        result.set("status", "active");
        result.stringify(response.send());
    }
    catch (const exception& e) { // ошибка транзакции, роллбэкаем
        session.rollback();
        response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Transaction failed");
        err.set("details", e.what());
        err.stringify(response.send());
    }
}
*/

void LoanHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    using namespace Poco::Data::Keywords;

    // 1. Проверка JWT
    auto auth = checkAuth(request, response);
    if (!auth.ok) return;

    // 2. Смотрим, есть ли в query параметр return
    Poco::URI uri(request.getURI());
    auto params = uri.getQueryParameters();
    string returnId;

    for (auto& p : params) {
        if (p.first == "return") returnId = p.second;
    }

    // 3. Если return есть — возвращаем книгу
    if (!returnId.empty()) {
        int loanId = 0;
        try {
            loanId = stoi(returnId);
        }
        catch (...) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Invalid loan id");
            err.stringify(response.send());
            return;
        }

        // ищем выдачу
        auto session = Database::getSession("Lichs.db");
        int bookId = 0, ownerId = 0;
        string status;

        session << "SELECT book_id, user_id, status FROM loans WHERE id = ?",
            use(loanId), into(bookId), into(ownerId), into(status), now;

        if (bookId == 0) {
            response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Loan not found");
            err.stringify(response.send());
            return;
        }

        if (ownerId != auth.userId) {
            response.setStatus(HTTPResponse::HTTP_FORBIDDEN);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "This loan belongs to another user");
            err.stringify(response.send());
            return;
        }

        if (status != "active") {
            response.setStatus(HTTPResponse::HTTP_CONFLICT);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Book is already returned");
            err.stringify(response.send());
            return;
        }

        // транзакция
        session.begin();
        try {
            session << "UPDATE loans SET status='returned', returned_at=datetime('now') WHERE id=?",
                use(loanId), now;
            session << "UPDATE books SET is_available=1 WHERE id=?",
                use(bookId), now;
            session.commit();

            response.setStatus(HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            Poco::JSON::Object result;
            result.set("id", loanId);
            result.set("book_id", bookId);
            result.set("status", "returned");
            result.stringify(response.send());
        }
        catch (const exception& e) {
            session.rollback();
            response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Transaction failed");
            err.set("details", e.what());
            err.stringify(response.send());
        }
        return;
    }

    // либо создаём выдачу книги если это не возврат
    Poco::JSON::Object::Ptr json;
    try {
        Poco::JSON::Parser parser;
        json = parser.parse(readBody(request)).extract<Poco::JSON::Object::Ptr>();
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Invalid JSON");
        err.stringify(response.send());
        return;
    }


    int bookId = 0;
    try {
        bookId = json->getValue<int>("book_id");
    }
    catch (...) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Field 'book_id' is required");
        err.stringify(response.send());
        return;
    }

    // поскольку здесь несколько запросов, то сделаем их одной транзакцией
    auto session = Database::getSession("Lichs.db");
    session.begin();

    try {
        // книга существует и доступна
        int available = -1;  // -1 = "не найдена"
        session << "SELECT is_available FROM books WHERE id = ?",
            use(bookId), into(available), now;

        if (available == -1) {
            session.rollback();// откат транзакции
            response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Book not found");
            err.stringify(response.send());
            return;
        }

        if (available == 0) {
            session.rollback();
            response.setStatus(HTTPResponse::HTTP_CONFLICT);
            response.setContentType("application/json");
            Poco::JSON::Object err;
            err.set("error", "Book is already issued");
            err.stringify(response.send());
            return;
        }

        // отметили что выдали книгу
        session << "UPDATE books SET is_available = 0 WHERE id = ?",
            use(bookId), now;

        // создаём книжный должок 
        session << "INSERT INTO loans (book_id, user_id, status, due_at) "
            "VALUES (?, ?, 'active', datetime('now', '+30 days'))",
            use(bookId), use(auth.userId), now;

        Poco::Int64 loanId = 0;
        session << "SELECT last_insert_rowid()", into(loanId), now;

        // коммит транзакции
        session.commit();

        response.setStatus(HTTPResponse::HTTP_CREATED);
        response.setContentType("application/json");
        Poco::JSON::Object result;
        result.set("id", static_cast<int>(loanId));
        result.set("book_id", bookId);
        result.set("user_id", auth.userId);
        result.set("login", auth.login);
        result.set("status", "active");
        result.stringify(response.send());
    }
    catch (const exception& e) { // ошибка транзакции, роллбэкаем
        session.rollback();
        response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        response.setContentType("application/json");
        Poco::JSON::Object err;
        err.set("error", "Transaction failed");
        err.set("details", e.what());
        err.stringify(response.send());
    }
}

void BookListHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    using namespace Poco::Data::Keywords;

    // раазбор параметров из запроса
    Poco::URI uri(request.getURI());
    auto params = uri.getQueryParameters();

    string titleFilter;
    string authorFilter;
    for (auto& p : params) {
        if (p.first == "title")  titleFilter = p.second;
        if (p.first == "author") authorFilter = p.second;
    }

    // теперь сам sql запрос
    auto session = Database::getSession("Lichs.db");
    Poco::Data::Statement select(session);

    string sql = "SELECT id, title, author, is_available FROM books";
    string whereClause;

    if (!titleFilter.empty() && !authorFilter.empty()) {
        whereClause = " WHERE title LIKE ? AND author LIKE ?";
    }
    else if (!titleFilter.empty()) {
        whereClause = " WHERE title LIKE ?";
    }
    else if (!authorFilter.empty()) {
        whereClause = " WHERE author LIKE ?";
    }

    sql += whereClause; // здесь получается запрос с нужными нам фильтрами теперь к нему надо добавить сами значения фильтров


    if (!titleFilter.empty() && !authorFilter.empty()) {
        titleFilter = "%" + titleFilter + "%";
        authorFilter = "%" + authorFilter + "%";
        select << sql, use(titleFilter), use(authorFilter);
    }
    else if (!titleFilter.empty()) {
        titleFilter = "%" + titleFilter + "%";
        select << sql, use(titleFilter);
    }
    else if (!authorFilter.empty()) {
        authorFilter = "%" + authorFilter + "%";
        select << sql, use(authorFilter);
    }
    else {
        select << sql;
    }

    select.execute();

    // получение резов
    Poco::Data::RecordSet rs(select);
    Poco::JSON::Array books;

    for (auto& row : rs) {
        Poco::JSON::Object book;
        book.set("id", row["id"].convert<int>());
        book.set("title", row["title"].toString());
        book.set("author", row["author"].toString());
        book.set("is_available", row["is_available"].convert<int>() == 1);
        books.add(book);
    }

    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    Poco::JSON::Object result;
    result.set("count", static_cast<int>(books.size()));
    result.set("books", books);
    result.stringify(response.send());
}