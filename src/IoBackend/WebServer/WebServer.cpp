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
#include "WebSocketProtokoll.hpp"

// 0 for unlimited
#define MAX_BUFFER_SIZE 0

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
    el::Helpers::setThreadName("WebServer MainLoop");
    LOG(DEBUG) << "WebServer Main Loop running";

    while(_run){
        if( lws_service(_context, 0) < 0 ) {
            LOG(ERROR) << "Error polling for socket activity.";
        }
    }
    LOG(DEBUG) << "WebServer Main Loop leaved";
}

WebServer::WebServer(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
    _serverPort = 8081;
    _run = true;
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

static const struct lws_http_mount basemount = {
	/* .mount_next */		    NULL,		/* linked-list "next" */
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
    { "http", callback_main, 0, 0, 0, NULL, 0 },//{ "http", lws_callback_http_dummy, 0, 0, 0, NULL, 0 },
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

    int logs = LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_USER | LLL_INFO | LLL_PARSER
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

/*#ifndef LWS_NO_EXTENSIONS
    info.extensions = lws_get_internal_extensions( );
#endif*/
    if( !_certPath.empty( ) && !_keyPath.empty( ) )
    {
        LOG(DEBUG) << "Using SSL certPath=" + _certPath + ". keyPath=" + _keyPath + ".";
        info.ssl_cert_filepath        = _certPath.c_str( );
        info.ssl_private_key_filepath = _keyPath.c_str( );
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
    info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
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

    if(_loopThread.joinable()) {
        _loopThread.join();
    }

    lws_context_destroy(_context);
    _context = nullptr;
}

bool RegisterResource(const std::string& resourceString, HttpResource* resourceClass)
{
    return false;
}

int WebServer::MainCallBack(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    char buf[512];
    switch (reason) {
        case LWS_CALLBACK_HTTP:
            //This Code ist Called when no File for Url is Found
            lws_get_peer_simple(wsi, buf, sizeof(buf));
            LOG(INFO) << "peer_simple " << buf;
		    //lwsl_notice("%s: HTTP: connection %s, path %s\n", __func__,	(const char *)buf, pss->path);

            if (lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_GET_URI) > 0) {
                LOG(INFO) << "GET Url " << buf;
            } else if(lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_POST_URI) > 0) {
                LOG(INFO) << "POST Url " << buf;
            } else if(lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_PATCH_URI) > 0) {
                LOG(INFO) << "PATCH Url " << buf;                
            } else if(lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_PUT_URI) > 0) {
                LOG(INFO) << "PUT Url " << buf;
            } else if(lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_DELETE_URI) > 0) {
                LOG(INFO) << "DELETE Url " << buf;
            } else {
                LOG(WARNING) << "Unkown Url Type";
            }
            break;
        default:
            //Noting todo
            break;
    }
    
    return lws_callback_http_dummy(wsi, reason, user, in, len);
}