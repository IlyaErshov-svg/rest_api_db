#include "demo_v1_provider.h"

using namespace demo::v1;

void provider::get_list(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                        int id) {
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("SELECT name, weight, license FROM detail WHERE fk_sup = $1;",
                                           id);
    Json::Value jsonArray;
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        for (auto row : result)
        {
            Json::Value tempObject;
            tempObject["name"] = row["name"].as<std::string>();
            tempObject["weight"] = row["weight"].as<std::string>();
            tempObject["license"] = row["license"].as<std::string>();
            jsonArray.append(std::move(tempObject));
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
    }
    auto resp = HttpResponse::newHttpJsonResponse(jsonArray);
    resp->setStatusCode(k200OK);
    callback(resp);
}


void provider::add_detail(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonBody = *req->getJsonObject();
    Json::Value response;
    if(!(jsonBody.isMember("name") && jsonBody.isMember("weight")
         && jsonBody.isMember("provider_id") ))
    {
        response["message"] = "Invalid data";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("INSERT INTO detail(name, fk_sup, weight) "
                                           "VALUES($1, $2, $3 )",
                                           jsonBody["name"].asString(),  jsonBody["provider_id"].asString(),
                                           jsonBody["weight"].asString());
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        response["message"] = "Success";
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        response["message"] = "Some wrong";
    }
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    callback(resp);
}
// Add definition of your processing function here
