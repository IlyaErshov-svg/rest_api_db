#include "demo_v1_mench.h"

using namespace demo::v1;

void mench::get_product(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                        std::string &&name) {
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("SELECT product.id, product.name as product_name, types.name as product_type,"
                                           " produced from product"
                                           " join types on product.fk_type = types.id\n"
                                           "where product.name = $1;",
                                           name);
    Json::Value jsonResult;
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        if(!result.empty())
        {
            jsonResult["id"] = result[0]["id"].as<std::string>();
            jsonResult["product_name"] = result[0]["product_name"].as<std::string>();
            jsonResult["type_name"] = result[0]["product_type"].as<std::string>();
            jsonResult["produced"] = result[0]["produced"].as<std::string>();
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
    }
    auto resp = HttpResponse::newHttpJsonResponse(jsonResult);
    resp->setStatusCode(k200OK);
    callback(resp);
}

void mench::update_product(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonBody = *req->getJsonObject();
    Json::Value response;
    if(!(jsonBody.isMember("id") && jsonBody.isMember("product_name")
         && jsonBody.isMember("type_name") &&  jsonBody.isMember("produced") ))
    {
        response["message"] = "Invalid data";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("UPDATE product SET name = $1, produced = $2,"
                                           "fk_type = (SELECT id from types where name = $3) "
                                           "WHERE product.id = $4",
                                           jsonBody["product_name"].asString(),  jsonBody["produced"].asString(),
                                           jsonBody["type_name"].asString(), jsonBody["id"].asString());
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

void mench::get_detail(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                       std::string &&name_detail, std::string &&name_provider) {
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("select detail.id, detail.name as detail_name, "
                                           "supplier.name as provider_name, weight, license "
                                           "from detail join supplier on detail.fk_sup = supplier.id\n"
                                           "where supplier.name = $1 and  detail.name = $2",
                                           name_provider, name_detail);
    Json::Value jsonResult;
    try
    {
        auto result = f.get(); // Block until we get the result or catch the exception;
        if(!result.empty())
        {
            jsonResult["id"] = result[0]["id"].as<std::string>();
            jsonResult["detail_name"] = result[0]["detail_name"].as<std::string>();
            jsonResult["provider_name"] = result[0]["provider_name"].as<std::string>();
            jsonResult["weight"] = result[0]["weight"].as<std::string>();
            jsonResult["license"] = result[0]["license"].as<std::string>();
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
    }
    auto resp = HttpResponse::newHttpJsonResponse(jsonResult);
    resp->setStatusCode(k200OK);
    callback(resp);
}

void mench::update_detail(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonBody = *req->getJsonObject();
    Json::Value response;
    if(!(jsonBody.isMember("id") && jsonBody.isMember("license")))
    {
        response["message"] = "Invalid data";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }
    auto clientPtr = drogon::app().getDbClient();
    auto f = clientPtr->execSqlAsyncFuture("UPDATE detail SET license = $1"
                                           "WHERE id = $2",
                                           jsonBody["license"].asString(),  jsonBody["id"].asString());
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


void mench::add_product(const drogon::HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonBody = *req->getJsonObject();
    Json::Value response;
    response["message"] = "Success";
    auto clientPtr = drogon::app().getDbClient();
    //auto transPtr = clientPtr->newTransaction();
    /*
    clientPtr->execSqlAsync("BEGIN TRANSACTION;",
                            [](){std::cout << "Start transaction" << std::endl;},
                            [=](const drogon::orm::DrogonDbException &e) mutable {
                                std::cerr << "error:" << e.base().what() << std::endl;
                                response["message"] = "Some wrong";
    });
    */
    clientPtr->execSqlAsync("INSERT INTO product(name, fk_type, produced)\n"
                            "VALUES($1, (SELECT id from types where name = $2), FALSE);",
                            [](){std::cerr << "Insert product" << std::endl;},
                            [=](const drogon::orm::DrogonDbException &e) mutable {
                                std::cerr << "error:" << e.base().what() << std::endl;
                                response["message"] = "Some wrong";
                            },
                            jsonBody[0]["name"].asString(), jsonBody[0]["type"].asString());
    unsigned int index_product;
    clientPtr->execSqlAsync("select id from product where name = $1;",
                            [&index_product](const drogon::orm::Result &result)mutable
                            {std::cerr << "Get index" << std::endl;
                                index_product = result[0]["id"].as<unsigned int>();},
                            [=](const drogon::orm::DrogonDbException &e) mutable {
                                std::cerr << "error:" << e.base().what() << std::endl;
                                response["message"] = "Some wrong";
                            },
                            jsonBody[0]["name"].asString());
    for(unsigned int i = 1; jsonBody.size(); ++i)
    {
        clientPtr->execSqlAsync("INSERT INTO specific(fk_product, fk_detail, amount)\n"
                                "VALUES($1, (SELECT detail.id from detail join supplier on detail.fk_sup = supplier.id "
                                "where detail.name = $2 and supplier.name = $3), $4);",
                                [i](){std::cerr << "Insert detail: " << i << std::endl;},
                                [=](const drogon::orm::DrogonDbException &e) mutable {
                                    std::cerr << "error:" << e.base().what() << std::endl;
                                    response["message"] = "Some wrong";
                                },
                                index_product, jsonBody[i]["detail_name"].asString(),
                                jsonBody[i]["provider_name"].asString(), jsonBody[i]["amount"].asInt());
    }
    /*
    clientPtr->execSqlAsync("COMMIT TRANSACTION;",
                            [](){std::cout << "end transaction" << std::endl;},
                            [=](const drogon::orm::DrogonDbException &e) mutable {
                                std::cerr << "error:" << e.base().what() << std::endl;
                                response["message"] = "Some wrong";
                            });
    */
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    callback(resp);
}
// Add definition of your processing function here
