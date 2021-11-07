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
    }
	char path[128];
    char responseData[2048];
    HttpResponse* response;
};


typedef void (*log_emit_function)(int,const char *);

void WebServer::LogCallBack(int level, const char *line)
{
    std::string traceLine(line);
    utils::replaceAll(traceLine, "\n", "");
    utils::replaceAll(traceLine, "\r", "");

    LOG(DEBUG) << traceLine;
}

void WebServer::MainLoop()
{
    el::Helpers::setThreadName("WebServer WorkerThread");
    LOG(DEBUG) << "WebServer Main Loop running";

    while(_run){
        if( lws_service(_context, 0) < 0 ) {
            LOG(ERROR) << "Error polling for socket activity.";
            break;
        }
    }
    LOG(DEBUG) << "WebServer Main Loop leaved";
}

HttpResponse* WebServer::HandleResource(struct lws *wsi, const std::string& url)
{
    const auto resource = _httpResources.find(url);
    if( resource == _httpResources.end()) {
        return nullptr;
    }

    HttpRequest request(wsi);
    try
    {
        return resource->second->Process(request);
    }
    catch(const std::exception& exp)
    {
        LOG(ERROR) << exp.what();
        return nullptr;
    }
}

WebServer::WebServer(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
    _serverPort = 8081;
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

static const struct lws_http_mount basemount = {
	/* .mount_next */		    &proxymount,		/* linked-list "next" */
	/* .mountpoint */		    "/",		/* mountpoint URL */
	/* .origin */			    "./webpage", /* serve from dir */
	/* .def */			        "index.html",	/* default filename */
	/* .protocol */			    NULL,
	/* .cgienv */			    NULL,
	/* .extra_mimetypes */	    NULL,
	/* .interpret */		    NULL,
	/* .cgi_timeout */		    0,
	/* .cache_max_age */		0,
	/* .auth_mask */		    0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_FILE,	/* files in a dir */
	/* .mountpoint_len */		1,		/* char count */
	/* .basic_auth_login_file*/	NULL,
};


static struct lws_protocols protocols[] = {
    { "http", callback_main, sizeof(struct httpSesssionData), 0, 0, NULL, 0 },//{ "http", lws_callback_http_dummy, 0, 0, 0, NULL, 0 },
    { "websocket",
        callback_websocket,
        sizeof(per_session_data__minimal),
        MAX_BUFFER_SIZE,
    },
    { NULL, NULL, 0, 0 } /* terminator */
};

bool WebServer::Start() {
    
    lws_context_creation_info info;
    memset( &info, 0, sizeof info );

    auto version = lws_get_library_version();
    LOG(INFO) << "LibWebSocket Version " << version;

    int logs = LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_USER | LLL_INFO 
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
    //info.error_document_404 = "/404.html";
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
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
    if(_httpResources.find(resourceString) != _httpResources.end()) {
        return false;
    }

    _httpResources.insert(std::pair<const std::string, HttpResource*>(resourceString, resourceClass));

    return true;
}

void WebServer::SendWebSocketBroadcast(const std::string& message)
{
    if(_webSocketVhostData == nullptr) return;

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

int WebServer::MainCallBack(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    auto found = false;
    char textBuffer[512];
    struct httpSesssionData *pss = (struct httpSesssionData *)user;
    
    //Todo Understand
    uint8_t buf[LWS_PRE + 2048], *start = &buf[LWS_PRE], *p = start, *end = &buf[sizeof(buf) - LWS_PRE - 1];

    switch (reason) {
        case LWS_CALLBACK_HTTP:
            //This Code ist Called when no File for Url is Found
            /*
            * In contains the url part after the place the mount was
            * positioned at, eg, if positioned at "/dyn" and given
            * "/dyn/mypath", in will contain /mypath
            */
		    lws_snprintf(pss->path, sizeof(pss->path), "%s",(const char *)in);

            lws_get_peer_simple(wsi, textBuffer, sizeof(textBuffer));
            LOG(INFO) << "peer_simple " << textBuffer;
		    //lwsl_notice("%s: HTTP: connection %s, path %s\n", __func__,	(const char *)buf, pss->path);

            
            if (lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_GET_URI) > 0) {
                LOG(INFO) << "GET Url " << textBuffer;
                found = true;
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_POST_URI) > 0) {
                LOG(INFO) << "POST Url " << textBuffer;
                found = true;
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_PATCH_URI) > 0) {
                LOG(INFO) << "PATCH Url " << textBuffer;
                found = true;                
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_PUT_URI) > 0) {
                LOG(INFO) << "PUT Url " << textBuffer;
                found = true;
            } else if(lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_DELETE_URI) > 0) {
                LOG(INFO) << "DELETE Url " << textBuffer;
                found = true;
            } else {
                LOG(WARNING) << "Unkown Url Type";
            }
            break;

        case LWS_CALLBACK_HTTP_WRITEABLE:
    		if (!pss || pss->response == nullptr)
			    break;
            {
                auto protState = LWS_WRITE_HTTP;
                if(pss->response->GetNextBlock(pss->responseData)) {
                    protState = LWS_WRITE_HTTP_FINAL;
                }

                p += lws_snprintf((char *)p, lws_ptr_diff_size_t(end, p), pss->responseData);

                if (lws_write(wsi, (uint8_t *)start, lws_ptr_diff_size_t(p, start), protState) != lws_ptr_diff(p, start)) {
                    //Error
                    return 1;
                }

                /*
                * HTTP/1.0 no keepalive: close network connection
                * HTTP/1.1 or HTTP1.0 + KA: wait / process next transaction
                * HTTP/2: stream ended, parent connection remains up
                */
                if (protState == LWS_WRITE_HTTP_FINAL) {
                    if (lws_http_transaction_completed(wsi))
                    return -1;
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
		    break;
        default:
            //Noting todo
            break;
    }
    
    if(found) {
        const auto response = HandleResource(wsi, textBuffer);
        if(response != nullptr)
        {
            pss->response = response;
            if (lws_add_http_common_headers(wsi, HTTP_STATUS_OK, response->GetContentType().c_str(), LWS_ILLEGAL_HTTP_CONTENT_LEN, /* no content len */ &p, end))
            {
                return 1;
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
        }
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

void WebServer::NewWebSocketClient()
{
    LOG(INFO) << "NewWebSocketClient";
}

void WebServer::WebSocketClientMessage(const std::string& message)
{
    LOG(INFO) << "WebSocketClientMessage " << message;
}

void WebServer::WebSocketInitDone(per_vhost_data__minimal* webSocketVhostData)
{
    _webSocketVhostData = webSocketVhostData;
}