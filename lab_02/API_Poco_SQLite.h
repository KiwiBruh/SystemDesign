// API_Poco_SQLite.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.
#pragma once
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/JSON/Object.h>
#include <Poco/Util/ServerApplication.h>

#include <iostream>


#include "Handlers.h"


using namespace std;
using namespace Poco::Net;

class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& request) override;
};
