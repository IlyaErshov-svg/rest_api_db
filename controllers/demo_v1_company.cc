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

void company::get_product(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string&& name) {
    auto clientPtr = drogon::app().getDbClient();
    name = "%" + name + "%";
    auto f = clientPtr->execSqlAsyncFuture("SELECT product.name as product_name, types.name as type from"
                                           " product join types on product.fk_type = types.id\n"
                                           "where product.produced = TRUE AND product.name like $1;",
                                           name);
    Json::Value jsonArray;
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        for (auto row : result)
        {
            Json::Value tempObject;
            tempObject["name"] = row["product_name"].as<std::string>();
            tempObject["type"] = row["type"].as<std::string>();
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


void company::post_delivery(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonBody = *req->getJsonObject();
    Json::Value response;
    if(!(jsonBody.isMember("amount") && jsonBody.isMember("product")
    && jsonBody.isMember("date") && jsonBody.isMember("client")))
    {
        response["message"] = "Invalid data";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("INSERT INTO delivery(fk_product, fk_client, amount, date) "
                                           "VALUES((SELECT id from product where name = $1),"
                                           " (SELECT id from client where name = $2), $3, $4 )",
                                           jsonBody["product"].asString(),  jsonBody["client"].asString(),
                                           jsonBody["amount"].asString(), jsonBody["date"].asString());
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