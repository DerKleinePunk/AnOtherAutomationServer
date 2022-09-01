#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "WebServer"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "WebServer.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"
#include <functional>
#include <libwebsockets.h>

// 0 for unlimited
#define MAX_BUFFER_SIZE 0

/*
 * Unlike ws, http is a stateless protocol.  This pss only exists for the
 * duration of a single http transaction.  With http/1.1 keep-alive and http/2,
 * that is unrelated to (shorter than) the lifetime of the network connection.
 */
struct httpSesssionData {

    httpSesssionData() {
        response = nullptr;
        body = nullptr;
        bodyLength = 0;
        method = nullptr;
        url = nullptr;
    }
    char responseData[2048];
    HttpResponse* response;
    std::string* body;
    int bodyLength;
    std::string* method;
    std::string* url;
};


typedef void (*log_emit_function)(int,const char *);

void WebServer::LogCallBack(int level, const char *line)
{
    std::string traceLine(line);
    utils::replaceAll(traceLine, "\n", "");
    utils::replaceAll(traceLine, "\r", "");
    
    //Todo Remove Date and interprat Level
    if(!VLOG_IS_ON(5)) return;
    traceLine = traceLine.substr(27);

    switch (level)
    {
        case LLL_ERR:
            LOG(ERROR) << traceLine.substr(3);
            break;
        case LLL_WARN:
            LOG(WARNING) << traceLine.substr(3);
            break;
        case LLL_INFO:
            LOG(INFO) << traceLine.substr(3);
            break;
        case LLL_DEBUG:
            LOG(DEBUG) << traceLine.substr(3);
            break;
        default:
            VLOG(5) << traceLine;
    }
}

void WebServer::MainLoop()
{
    el::Helpers::setThreadName("WebServer WorkerThread");
    LOG(DEBUG) << "WebServer Main Loop running";

    try
    {
        while(_run){
            if( lws_service(_context, 0) < 0 ) {
                LOG(ERROR) << "Error polling for socket activity.";
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << e.what();
    }
        
    LOG(DEBUG) << "WebServer Main Loop leaved";
}

HttpResponse* WebServer::HandleResource(struct lws *wsi, const std::string& url, const std::string& method, const std::string* body)
{
    HttpResource* resource = nullptr;
    auto urlForResource = url;

    auto resourceIter = _httpResources.find(url);
    if( resourceIter == _httpResources.end()) {
        for(auto entry : _httpResources) {
            if(entry.second.EndWildcard && utils::hasBegining(url, entry.first)) {
                resource = entry.second.Implementation;
                urlForResource = urlForResource.substr(entry.first.size());
                LOG(DEBUG) << "Implementation found over begining " << entry.first;
                break;
            }
        }
        if(resource == nullptr)
        {
            return nullptr;
        }
    } else {
        resource = resourceIter->second.Implementation;
        urlForResource = urlForResource.substr(resourceIter->first.size());
    }

    HttpRequest request(wsi, body);
    try
    {
        const auto result = resource->Process(request, urlForResource, method);
        //Todo Put it to Config
        //Handle Cors Headers global
        result->SetHeader("Access-Control-Allow-Origin", "http://localhost:8000");
        result->SetHeader("Access-Control-Allow-Credentials", "true");
        result->SetHeader("Access-Control-Allow-Methods", "DELETE, POST, GET, PUT");
        result->SetHeader("Access-Control-Allow-Headers", "Content-Type");

        return result;
    }
    catch(const std::exception& exp)
    {
        LOG(ERROR) << exp.what();
        return nullptr;
    }
}

std::string WebServer::GetReasonText(lws_callback_reasons reason)
{
    switch (reason)
    {
        case LWS_CALLBACK_HTTP:
            return "LWS_CALLBACK_HTTP";
        case LWS_CALLBACK_CLOSED_HTTP:
            return "LWS_CALLBACK_CLOSED_HTTP";
        case LWS_CALLBACK_CLIENT_WRITEABLE:
            return "LWS_CALLBACK_CLIENT_WRITEABLE";
        case LWS_CALLBACK_HTTP_FILE_COMPLETION:
            return "LWS_CALLBACK_HTTP_FILE_COMPLETION";
        case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
            return "LWS_CALLBACK_EVENT_WAIT_CANCELLED";
        case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
            return "LWS_CALLBACK_FILTER_NETWORK_CONNECTION";
        default:
            return "reason " + std::to_string(reason);
    }

}

WebServer::WebServer(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
    _serverPort = _globalFunctions->GetServerPort();
    _run = true;
    _webSocketVhostData = nullptr;
}

WebServer::~WebServer()
{
    _run = false;
}

static int callback_main( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
    /*int fd;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];*/
    
    auto* webServer = (WebServer*) lws_context_user(lws_get_context(wsi));

    if(webServer){
        return webServer->MainCallBack(wsi, reason, user, in, len);
    }

    return 0;
}

static struct lws_protocol_vhost_options em3 = {
        NULL, NULL, ".zip", "application/zip"
}, em2 = {
	&em3, NULL, ".pdf", "application/pdf"
}, extra_mimetypes = {
	&em2, NULL, "*", "plain/text"
};

static const struct lws_http_mount proxymount = {
	/* .mount_next */		NULL,		/* linked-list "next" */
	/* .mountpoint */		"/ha",		/* mountpoint URL */
	/* .origin */			"localhost:7681/", /* serve from dir */
	/* .def */			    "index.html",	/* default filename */
	/* .protocol */			    NULL,
	/* .cgienv */			    NULL,
	/* .extra_mimetypes */		NULL,
	/* .interpret */		    NULL,
	/* .cgi_timeout */		    0,
	/* .cache_max_age */		0,
	/* .auth_mask */		    0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_HTTP,	/* files in a dir */
	/* .mountpoint_len */		3,		/* char count */
	/* .basic_auth_login_file */	NULL,
};

static const struct lws_http_mount mountWebsocket = {
	/* .mount_next */		    NULL,		/* linked-list "next" */
	/* .mountpoint */		    "/messages",		/* mountpoint URL */
	/* .origin */			    NULL, /* serve from dir */
	/* .def */			        NULL,	/* default filename */
	/* .protocol */			    "websocket",
	/* .cgienv */			    NULL,
	/* .extra_mimetypes */		NULL,
	/* .interpret */		    NULL,
	/* .cgi_timeout */		    0,
	/* .cache_max_age */		0,
	/* .auth_mask */		    0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_CALLBACK,	/* files in a dir */
	/* .mountpoint_len */		9,		/* char count */
	/* .basic_auth_login_file */	NULL,
};

static const struct lws_http_mount webpage = {
	/* .mount_next */		    &mountWebsocket,		/* linked-list "next" &proxymount*/
	/* .mountpoint */		    "/",		/* mountpoint URL */
	/* .origin */			    "./webpage", /* serve from dir */
	/* .def */			        "index.html",	/* default filename */
	/* .protocol */			    NULL,
	/* .cgienv */			    NULL,
	/* .extra_mimetypes */	    &extra_mimetypes,
	/* .interpret */		    NULL,
	/* .cgi_timeout */		    0,
	/* .cache_max_age */		3600, //1 Stunde
	/* .auth_mask */		    0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_FILE,	/* files in a dir */
	/* .mountpoint_len */		1,		/* char count */
	/* .basic_auth_login_file*/	NULL,
};

static const struct lws_http_mount basemount = {
	/* .mount_next */		    &webpage,		/* linked-list "next" &proxymount*/
	/* .mountpoint */		    "/api",		/* mountpoint URL */
	/* .origin */			    NULL, /* serve from dir */
	/* .def */			        NULL,	/* default filename */
	/* .protocol */			    "httpRest",
	/* .cgienv */			    NULL,
	/* .extra_mimetypes */	    &extra_mimetypes,
	/* .interpret */		    NULL,
	/* .cgi_timeout */		    0,
	/* .cache_max_age */		3600, //1 Stunde
	/* .auth_mask */		    0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_CALLBACK,	/* files in a dir */
	/* .mountpoint_len */		4,		/* char count */
	/* .basic_auth_login_file*/	NULL,
};

static struct lws_protocols protocols[] = {
    { "http", callback_main, sizeof(struct httpSesssionData), MAX_BUFFER_SIZE, 1, NULL, 0 },//{ "http", lws_callback_http_dummy, 0, 0, 0, NULL, 0 },
    { "httpRest", callback_main, sizeof(struct httpSesssionData), MAX_BUFFER_SIZE, 2, NULL, 0 },
    { "websocket", callback_websocket, sizeof(per_session_data__minimal), MAX_BUFFER_SIZE, 3, NULL, 0 },
    LWS_PROTOCOL_LIST_TERM
};

/**
 * @brief Starts the Webserver
 * 
 * @return true 
 * @return false 
 */
bool WebServer::Start() {
    
    lws_context_creation_info info;
    memset( &info, 0, sizeof info );

    auto version = lws_get_library_version();
    LOG(INFO) << "LibWebSocket Version " << version;

    int logs = LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_USER | LLL_INFO | LLL_DEBUG
			/* for LLL_ verbosity above NOTICE to be built into lws,
			 * lws must have been configured and built with
			 * -DCMAKE_BUILD_TYPE=DEBUG instead of =RELEASE */
			/* |  */ /* | LLL_PARSER */ /* | LLL_HEADER */
			/* | LLL_EXT */ /* | LLL_CLIENT */ /* | LLL_LATENCY */
			/* | LLL_DEBUG */;

    utils::Callback<void(int,const char *)>::func = std::bind(&WebServer::LogCallBack, this, std::placeholders::_1, std::placeholders::_2);
    log_emit_function func = static_cast<log_emit_function>(utils::Callback<void(int,const char *)>::callback);

    lws_set_log_level(logs, func);

    info.port = _serverPort;
    info.iface = NULL;
    info.protocols = protocols;
    info.mounts = &basemount;
    //info.mounts = &proxymount;

/*#ifndef LWS_NO_EXTENSIONS
    info.extensions = lws_get_internal_extensions( );
#endif*/
    if( !_certPath.empty( ) && !_keyPath.empty( ) )
    {
        LOG(DEBUG) << "Using SSL certPath=" + _certPath + ". keyPath=" + _keyPath + ".";
        info.ssl_cert_filepath        = _certPath.c_str();
        info.ssl_private_key_filepath = _keyPath.c_str();
    }
    else
    {
        LOG(DEBUG) <<  "Not using SSL";
        info.ssl_cert_filepath        = NULL;
        info.ssl_private_key_filepath = NULL;
    }
    info.gid = -1;
    info.uid = -1;
    info.error_document_404 = "/404.html";
    //Todo put T to Config
    //info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.user = this;

    // keep alive
    info.ka_time = 60; // 60 seconds until connection is suspicious
    info.ka_probes = 10; // 10 probes after ^ time
    info.ka_interval = 10; // 10s interval for sending probes

    _context = lws_create_context( &info );
    if( !_context) {
        LOG(ERROR) << "libwebsocket init failed";
        return false;
    }

    LOG(DEBUG) << "Webserver Started add " << _serverPort;

    _loopThread = std::thread(&WebServer::MainLoop, this);
    return true;
}

void WebServer::Deinit()
{
    _run = false;
    
    LOG(INFO) << "Waiting Shutdown Webserver";
    lws_cancel_service(_context);

    if(_loopThread.joinable()) {
        _loopThread.join();
    }

    lws_context_destroy(_context);
    _context = nullptr;
}

bool WebServer::RegisterResource(const std::string& resourceString, HttpResource* resourceClass)
{
    auto resourceUrl = resourceString;
    HttpResourceInfo info;

    if(utils::hasEnding(resourceUrl, "/*"))
    {
        resourceUrl = resourceUrl.substr(0, resourceUrl.length() - 1);
        info.EndWildcard = true;
    }

    if(_httpResources.find(resourceUrl) != _httpResources.end()) {
        return false;
    }
    
    //Todo Check this api/* and api/test/* is not OK

    info.Implementation = resourceClass;
   
    _httpResources.insert(std::pair<const std::string, HttpResourceInfo>(resourceUrl, info));

    return true;
}

void WebServer::SendWebSocketBroadcast(const std::string& message)
{
    if(_webSocketVhostData == nullptr) {
        LOG(ERROR) << "_webSocketVhostData == nullptr";
        return;
    }

    if(_webSocketClients.size() == 0) {
        LOG(DEBUG) << "No WebSocket client no send";
        return;
    }

    msg amsg;
    amsg.len = message.size();
    amsg.payload = malloc(LWS_PRE + amsg.len); // Now new because protokoll impemention is C

    memcpy((char *)amsg.payload + LWS_PRE, message.c_str(), amsg.len);
    if (!lws_ring_insert(_webSocketVhostData->ring, &amsg, 1)) {
        /*__minimal_destroy_message(&amsg);
        lwsl_user("dropping 2!\n");
        break;*/
        free(amsg.payload);
        LOG(ERROR) << "no free buffer in ring";
        return;
    }

     /* let every subscriber know we want to write something
        * on them as soon as they are ready
        */
    lws_start_foreach_llp(struct per_session_data__minimal **,
                    ppss, _webSocketVhostData->pss_list) {
        if (!(*ppss)->publishing)
            lws_callback_on_writable((*ppss)->wsi);
    } lws_end_foreach_llp(ppss, pss_list);
}

void WebServer::SendWebSocket(const std::string& message, uuid_t connectionId)
{
    if(_webSocketVhostData == nullptr) {
        LOG(ERROR) << "_webSocketVhostData == nullptr";
        return;
    }

    if(_webSocketClients.size() == 0) {
        LOG(DEBUG) << "No WebSocket client no send";
        return;
    }

    msg amsg;
    amsg.len = message.size();
    amsg.payload = malloc(LWS_PRE + amsg.len); // Now new because protokoll impemention is C

    memcpy((char *)amsg.payload + LWS_PRE, message.c_str(), amsg.len);
    if (!lws_ring_insert(_webSocketVhostData->ring, &amsg, 1)) {
        /*__minimal_destroy_message(&amsg);
        lwsl_user("dropping 2!\n");
        break;*/
        free(amsg.payload);
        LOG(ERROR) << "no free buffer in ring";
        return;
    }

     /* let every subscriber know we want to write something
        * on them as soon as they are ready
        */
    lws_start_foreach_llp(struct per_session_data__minimal **,
                    ppss, _webSocketVhostData->pss_list) {
        if (!(*ppss)->publishing && (*ppss)->connectionId == connectionId) {
            lws_callback_on_writable((*ppss)->wsi);
        }
    } lws_end_foreach_llp(ppss, pss_list);
}

int WebServer::MainCallBack(lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    auto weHandleIt = false;
    auto readyToHandle = false;

    char textBuffer[512];
    char textBuffer2[512];
    struct httpSesssionData *pss = (struct httpSesssionData *)user;
    
    auto protocolId = -1;
    const auto protocol = lws_get_protocol(wsi);
    if(protocol != nullptr) {
        protocolId = protocol->id;
    }

    VLOG(5) << "MainCallBack " << GetReasonText(reason) << " protocol id " << std::to_string(protocolId);

    //Todo Understand
    uint8_t buf[LWS_PRE + 2048], *start = &buf[LWS_PRE], *p = start, *end = &buf[sizeof(buf) - LWS_PRE - 1];

    switch (reason) {
        case LWS_CALLBACK_HTTP:
            //This Code ist Called when no File for Url is Found
            /*
            * In contains the url part after the place the mount was
            * positioned at, eg, if positioned at "/dyn" and given
            * "/dyn/mypath", in will contain /mypath
            * lws_snprintf(pss->path, sizeof(pss->path), "%s",(const char *)in);
            */
		    
            lws_get_peer_simple(wsi, textBuffer, sizeof(textBuffer));
            LOG(INFO) << "peer_simple " << textBuffer;
		    //lwsl_notice("%s: HTTP: connection %s, path %s\n", __func__,	(const char *)buf, pss->path);

            
            if (lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_GET_URI) > 0) {
                LOG(INFO) << "GET Url " << textBuffer;
                pss->method = new std::string("GET");
                pss->url = new std::string(textBuffer);
                weHandleIt = true;
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_POST_URI) > 0) {
                if(lws_hdr_copy(wsi, textBuffer2, sizeof(textBuffer2), WSI_TOKEN_HTTP_CONTENT_LENGTH) > 0) {
                    pss->bodyLength = atoi(textBuffer2);
                    LOG(INFO) << "POST Url " << textBuffer << " content-length " << textBuffer2;
                } else {
                    LOG(INFO) << "POST Url " << textBuffer;
                }
                pss->method = new std::string("POST");
                pss->url = new std::string(textBuffer);
                weHandleIt = true;
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_PATCH_URI) > 0) {
                if(lws_hdr_copy(wsi, textBuffer2, sizeof(textBuffer2), WSI_TOKEN_HTTP_CONTENT_LENGTH) > 0) {
                    pss->bodyLength = atoi(textBuffer2);
                    LOG(INFO) << "PATCH Url " << textBuffer << " content-length " << textBuffer2;
                } else {
                    LOG(INFO) << "PATCH Url " << textBuffer;
                }
                pss->method = new std::string("PATCH");
                pss->url = new std::string(textBuffer);
                weHandleIt = true;                
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_PUT_URI) > 0) {
                if(lws_hdr_copy(wsi, textBuffer2, sizeof(textBuffer2), WSI_TOKEN_HTTP_CONTENT_LENGTH) > 0) {
                    pss->bodyLength = atoi(textBuffer2);
                    LOG(INFO) << "PUT Url " << textBuffer << " content-length " << textBuffer2;
                } else {
                    LOG(INFO) << "PUT Url " << textBuffer;
                }
                pss->method = new std::string("PUT");
                pss->url = new std::string(textBuffer);
                weHandleIt = true;
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_DELETE_URI) > 0) {
                LOG(INFO) << "DELETE Url " << textBuffer;
                pss->method = new std::string("DELETE");
                pss->url = new std::string(textBuffer);
                weHandleIt = true;
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_OPTIONS_URI) > 0) {
                LOG(INFO) << "OPTIONS Url " << textBuffer;
                pss->method = new std::string("OPTIONS");
                pss->url = new std::string(textBuffer);
                weHandleIt = true;
            } else {
                LOG(WARNING) << "Unkown Url Type";
            }

            if(weHandleIt && pss->bodyLength == 0) {
                readyToHandle = true;
            }

            break;
        case LWS_CALLBACK_HTTP_BODY:
            weHandleIt = true;
            if(pss->body == nullptr) {
                pss->body = new std::string();
            }
            pss->body->append((char*)in,len);
            break;
        case LWS_CALLBACK_HTTP_BODY_COMPLETION:
            weHandleIt = true;
            readyToHandle = true;
            break;
        case LWS_CALLBACK_HTTP_WRITEABLE:
    		if (!pss || pss->response == nullptr)
			    break;
            {
                auto protState = LWS_WRITE_HTTP;
                if(pss->response->GetNextBlock(pss->responseData)) {
                    protState = LWS_WRITE_HTTP_FINAL;
                }

                if(strlen(pss->responseData) > 0) {
                    p += lws_snprintf((char *)p, lws_ptr_diff_size_t(end, p), pss->responseData);

                    if (lws_write(wsi, (uint8_t *)start, lws_ptr_diff_size_t(p, start), protState) != lws_ptr_diff(p, start)) {
                        //Error
                        return 1;
                    }
                }

                /*
                * HTTP/1.0 no keepalive: close network connection
                * HTTP/1.1 or HTTP1.0 + KA: wait / process next transaction
                * HTTP/2: stream ended, parent connection remains up
                */
                if (protState == LWS_WRITE_HTTP_FINAL) {
                    if (lws_http_transaction_completed(wsi)) {
                        return -1;
                    }
                } else
                    lws_callback_on_writable(wsi);

                return 0;
            }
        case LWS_CALLBACK_CLOSED_CLIENT_HTTP:
        case LWS_CALLBACK_HTTP_DROP_PROTOCOL:
            /* called when our wsi user_space is going to be destroyed */
            if (pss->response != nullptr) {
                delete pss->response;
                pss->response = nullptr;
            }
            if (pss->body != nullptr) {
                delete pss->body;
                pss->body = nullptr;
            }
            if(pss->method != nullptr) {
                delete pss->method;
                pss->method = nullptr;
            }
            if(pss->url != nullptr) {
                delete pss->method;
                pss->url = nullptr;
            }
		    break;
        default:
            //Noting todo
            break;
    }
    
    if(weHandleIt) {
        if(readyToHandle) {
            const auto response = HandleResource(wsi, (*pss->url), (*pss->method), pss->body);
            if(response != nullptr)
            {
                pss->response = response;
                const char* contentType = nullptr;
                if(!response->GetContentType().empty())
                {
                    contentType = response->GetContentType().c_str();
                }

                if (lws_add_http_common_headers(wsi, response->GetCode(), contentType, LWS_ILLEGAL_HTTP_CONTENT_LEN, /* no content len */ &p, end))
                {
                    return 1;
                }
                               
                const auto headers = response->GetHeaders();
                for(auto header : headers) {
                    if(lws_add_http_header_by_name(wsi,(unsigned char*)header.first.c_str(), (unsigned char*)header.second.c_str(), header.second.length(), &p, end))
                    {
                        LOG(WARNING) << "failed write header";
                        return 1;
                    }
                }

                const auto cookies = response->GetCookies();
                for(auto cookie : cookies) {
                    /*n = lws_snprintf(temp, sizeof(temp), "__Host-%s=%s;"
                        "HttpOnly;"
                        "Secure;"
                        "SameSite=strict;"
                        "Path=/;"
                        "Max-Age=%lu",
                        i->cookie_name, plain, i->expiry_unix_time);
                        */
                    char temp[128];
                    //Todo Host Cookies scheit FireFox nur noch bei https zu unterstüzen
                    //int n = lws_snprintf(temp, sizeof(temp), "__Host-%s=%s;SameSite=Strict;Path=/;", cookie.first.c_str(),  cookie.second.c_str());
                    int n = lws_snprintf(temp, sizeof(temp), "%s=%s;SameSite=Strict;Path=/;", cookie.first.c_str(),  cookie.second.c_str());

                    if (lws_add_http_header_by_token(wsi, WSI_TOKEN_HTTP_SET_COOKIE, (uint8_t *)temp, n, &p, end)) {
                        //lwsl_err("%s: failed to add JWT cookie header\n", __func__);
                        LOG(WARNING) << "failed write cookie header";
                        return 1;
                    }
                }

                if (lws_finalize_write_http_header(wsi, start, &p, end))
                {
                    return 1;
                }

                //Todo other Things for response Session init
                //https://github.com/warmcat/libwebsockets/blob/v4.2-stable/minimal-examples/http-server/minimal-http-server-dynamic/minimal-http-server-dynamic.c

                /* write the body separately */
                lws_callback_on_writable(wsi);

                return 0;
            } else {
                //Todo find way HTTP_STATUS_INTERNAL_SERVER_ERROR or HTTP_STATUS_NOT_FOUND
                if (lws_add_http_common_headers(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, "text/html", LWS_ILLEGAL_HTTP_CONTENT_LEN, &p, end))
                {
                    return 1;
                }
                
                if (lws_finalize_write_http_header(wsi, start, &p, end))
                {
                    return 1;
                }

                lws_callback_on_writable(wsi);

                return 0;
            }
        }
        return 0;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

void WebServer::WebSocketInitDone(per_vhost_data__minimal* webSocketVhostData)
{
    _webSocketVhostData = webSocketVhostData;
}

void WebServer::NewWebSocketClient(bool consumer, uuid_t connectionId)
{
    char uuid_str[37]; // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
    uuid_unparse_upper(connectionId, uuid_str);
    const auto stringId = std::string(uuid_str);

    LOG(INFO) << "NewWebSocketClient " << consumer << " id " << stringId;

    WebSocketClient client;
    client.Type = "all";

    _webSocketClients.insert(std::pair<std::string, WebSocketClient>(stringId, client));
}

void WebServer::WebSocketClientMessage(const std::string& message, uuid_t connectionId)
{
    char uuid_str[37]; // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
    uuid_unparse_upper(connectionId, uuid_str);
    const auto stringId = std::string(uuid_str);

    LOG(INFO) << "WebSocketClientMessage " << message << " id " << stringId;

    if(message == "ECHO") {
        SendWebSocket("I Cant hear you", connectionId);
    }
}

void WebServer::RemoveWebSocketClient(bool consumer, uuid_t connectionId)
{
    char uuid_str[37]; // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
    uuid_unparse_upper(connectionId, uuid_str);
    const auto stringId = std::string(uuid_str);

    LOG(INFO) << "RemoveWebSocketClient " << consumer << " id " << stringId;
    if(_webSocketClients.find(stringId) != _webSocketClients.end()) {
        _webSocketClients.erase(stringId);
    }
}

bool WebServer::IsApiKeyOk(const std::string& value)
{
    return _globalFunctions->IsApiKeyOk(value);
}