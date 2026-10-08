#include "API_Poco_PG.h"


using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;


HTTPRequestHandler* HandlerFactory::createRequestHandler(const HTTPServerRequest& request) {
    string uri = request.getURI();

    // отрезаем query-часть для /books?author= или title если знака вопроса нет, то вернет всю строку, что поидее нас устраивает
    uri = uri.substr(0, uri.find('?'));

    const string& method = request.getMethod();

    if (uri == "/ping" && method == "GET")       return new PingHandler();
    if (uri == "/register" && method == "POST")  return new RegisterHandler();
    if (uri == "/login" && method == "POST")     return new LoginHandler();
    if (uri == "/books" && method == "POST")     return new BookCreateHandler();
    if (uri == "/loans" && method == "POST")     return new LoanHandler();
    if (uri == "/books" && method == "GET")      return new BookListHandler();

    return nullptr;
}



class LichServerApp : public ServerApplication {
protected:
    int main(const vector<string>& args) override {
        try {

            //Database::init("Lichs.db"); // сначала бд (это с путем для sqllite)
            Database::init();

            ServerSocket svs(8080);// порт
            HTTPServer srv(new HandlerFactory(), svs, new HTTPServerParams);// серыер

            srv.start();// запуск сервера


            waitForTerminationRequest();//крутится здесь ожидая условия на выход
            srv.stop();
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(LichServerApp);