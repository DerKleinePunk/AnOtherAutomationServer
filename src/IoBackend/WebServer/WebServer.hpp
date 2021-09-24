#pragma once

#include <libwebsockets.h>
#include <string>
#include <thread>
#include "../GlobalFunctions.hpp"

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

    void LogCallBack(int level, const char *line);
    void MainLoop();
public:
    WebServer(GlobalFunctions* globalFunctions);
    ~WebServer();

    bool Start();
    void Deinit();

    //Internal Use Only
    int MainCallBack(struct lws *wsi, enum lws_callback_reasons reason, void *in, size_t len);
};
