#include "demo_v1_analyst.h"

using namespace demo::v1;

void analyst::get_product(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                          int amount) {
    auto clientPtr = drogon::app().getDbClient();
    Json::Value arrayValue;
    auto f = clientPtr->execSqlAsyncFuture("SELECT product.id, product.name, types.name AS type_name, produced FROM product "
                                           "JOIN types on product.fk_type = types.id \n"
                                           "LIMIT $1",
                                           std::size_t(amount));
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        for (auto row : result)
        {
            Json::Value temp;
            temp["id"] = row["id"].as<std::size_t>();
            temp["name"] = row["name"].as<std::string>();
            temp["type_name"] = row["type_name"].as<std::string>();
            temp["produced"] = row["produced"].as<std::string>();
            arrayValue.append(temp);
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
    }
    auto resp = HttpResponse::newHttpJsonResponse(arrayValue);
    resp->setStatusCode(k200OK);
    callback(resp);
}
// Add definition of your processing function here
void analyst::get_detail(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                          int amount) {
    auto clientPtr = drogon::app().getDbClient();
    Json::Value arrayValue;
    auto f = clientPtr->execSqlAsyncFuture("SELECT * from detail\n"
                                           "LIMIT $1",
                                           std::size_t(amount));
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        for (auto row : result)
        {
            Json::Value temp;
            temp["id"] = row["id"].as<std::size_t>();
            temp["name"] = row["name"].as<std::string>();
            temp["id_provider"] = row["fk_sup"].as<std::size_t>();
            temp["weight"]  = row["weight"].as<double>();
            temp["license"] = row["license"].as<std::string>();
            arrayValue.append(temp);
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
    }
    auto resp = HttpResponse::newHttpJsonResponse(arrayValue);
    resp->setStatusCode(k200OK);
    callback(resp);
}