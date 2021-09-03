#pragma once

#include <httpserver.hpp>
#include "../../common/json/json.hpp"
#include "../GlobalFunctions.hpp"

class HtmlPageResource : public httpserver::http_resource
{
private:
    GlobalFunctions* _globalFunctions;
public:
    HtmlPageResource(GlobalFunctions* globalFunctions);
    ~HtmlPageResource();

    const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request&);
};

