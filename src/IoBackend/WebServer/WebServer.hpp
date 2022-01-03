#pragma once

#include <libwebsockets.h>
#include <string>
#include <thread>
#include <map>
#include "../GlobalFunctions.hpp"
#include "HttpResource.hpp"
#include "HttpResponse.hpp"
#include "WebSocketProtokoll.hpp"
#include <uuid/uuid.h>


struct HttpResourceInfo
{
    HttpResourceInfo() {
        Implementation = nullptr;
        EndWildcard = false;
    };

    HttpResource* Implementation;
    bool EndWildcard;
};

struct WebSocketClient
{
    std::string Type;
};

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
    std::map<const std::string , HttpResourceInfo> _httpResources;
    per_vhost_data__minimal* _webSocketVhostData;
    std::map<std::string, WebSocketClient> _webSocketClients;

    void LogCallBack(int level, const char *line);
    void MainLoop();
    HttpResponse* HandleResource(struct lws *wsi, const std::string& url, const std::string& method, const std::string* body);
public:
    WebServer(GlobalFunctions* globalFunctions);
    ~WebServer();

    bool Start();
    void Deinit();

    bool RegisterResource(const std::string& resourceString, HttpResource* resourceClass);
    void SendWebSocketBroadcast(const std::string& message);
    void SendWebSocket(const std::string& message, uuid_t connectionId);

    //Internal Use Only
    int MainCallBack(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

    void WebSocketInitDone(per_vhost_data__minimal* webSocketVhostData);
    void NewWebSocketClient(bool consumer, uuid_t connectionId);
    void WebSocketClientMessage(const std::string& message, uuid_t connectionId);
    void RemoveWebSocketClient(bool consumer, uuid_t connectionId);

    bool IsApiKeyOk(const std::string& value);
};
