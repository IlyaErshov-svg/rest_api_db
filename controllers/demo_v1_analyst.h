#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace demo
{
namespace v1
{
class analyst : public drogon::HttpController<analyst>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(analyst::get, "/{2}/{1}", Get); // path is /demo/v1/analyst/{arg2}/{arg1}
    // METHOD_ADD(analyst::your_method_name, "/{1}/{2}/list", Get); // path is /demo/v1/analyst/{arg1}/{arg2}/list
    // ADD_METHOD_TO(analyst::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    METHOD_ADD(analyst::get_product, "/product/{1}", Get);
    METHOD_ADD(analyst::get_detail, "/detail/{1}", Get);
    METHOD_LIST_END
    void get_product(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int amount);
    void get_detail(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int amount);
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
}
}
