#pragma once

#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>

using namespace drogon;

namespace demo
{
namespace v1
{
class company : public drogon::HttpController<company>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(company::get, "/{2}/{1}", Get); // path is /demo/v1/company/{arg2}/{arg1}
    // METHOD_ADD(company::your_method_name, "/{1}/{2}/list", Get); // path is /demo/v1/company/{arg1}/{arg2}/list
    // ADD_METHOD_TO(company::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    METHOD_ADD(company::get_list, "/list", Get);
    METHOD_ADD(company::hello, "/", Get);
    METHOD_LIST_END
    void get_list(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void hello(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
}
}
