#pragma once

#include <libwebsockets.h>
#include <string>
#include <thread>
#include <map>
#include "../GlobalFunctions.hpp"
#include "HttpResource.hpp"
#include "HttpResponse.hpp"

class WebServer
{
private:
    GlobalFunctions* _globalFunctions;
    lws_context* _context;
    int _serverPort;
    std::string _certPath;  
    std::string _keyPath;
    std::thread _loopThread;
    bool _run;
    std::map<const std::string , HttpResource*> _httpResources;

    void LogCallBack(int level, const char *line);
    void MainLoop();
    HttpResponse* HandleResource(struct lws *wsi, const std::string& url);
public:
    WebServer(GlobalFunctions* globalFunctions);
    ~WebServer();

    bool Start();
    void Deinit();

    bool RegisterResource(const std::string& resourceString, HttpResource* resourceClass);

    //Internal Use Only
    int MainCallBack(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
};
