#include "demo_v1_company.h"

using namespace demo::v1;


void company::hello(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp=HttpResponse::newHttpResponse();
    //NOTE: The enum constant below is named "k200OK" (as in 200 OK), not "k2000K".
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    resp->setBody("Welcome to company API!");
    callback(resp);
}

// Add definition of your processing function here
void company::get_list(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("select * from client");
    std::stringstream stream;
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        stream << result.size() << " rows selected!" << "\n";
        int i = 0;
        for (auto row : result)
        {
            stream << i++ << ": client name is " << row["name"].as<std::string>() << "\n";
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
    }
    auto resp=HttpResponse::newHttpResponse();
    //NOTE: The enum constant below is named "k200OK" (as in 200 OK), not "k2000K".
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    resp->setBody(stream.str());
    callback(resp);
}