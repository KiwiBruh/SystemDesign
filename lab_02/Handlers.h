#pragma once
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>
#include <Poco/JSON/Array.h>
#include <Poco/Data/Statement.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Util/ServerApplication.h>

#include <iostream>

#include "Database.h"
#include "jwtUtils.h"


class PingHandler : public Poco::Net::HTTPRequestHandler {  // просто пинг
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;
};

class RegisterHandler : public Poco::Net::HTTPRequestHandler { // регистрация новго пользователя
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;
};

class LoginHandler : public Poco::Net::HTTPRequestHandler { // вход здесь будем выдавать jwt
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;
};

class BookCreateHandler : public Poco::Net::HTTPRequestHandler { // добавление книг будет защищенным
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;
};

/*
class LoanCreateHandler : public Poco::Net::HTTPRequestHandler { // добавление долга по книге хз как это по-другому назвать
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;
};
*/

class BookListHandler : public Poco::Net::HTTPRequestHandler { // гет букс три варианта: все/по автору/по названию
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;
};

class LoanHandler : public Poco::Net::HTTPRequestHandler {  // возврат книги
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;
};