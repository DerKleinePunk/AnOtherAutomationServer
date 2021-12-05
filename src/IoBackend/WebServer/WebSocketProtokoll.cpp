#include "WebSocketProtokoll.hpp"
#include "WebServer.hpp"
#include <string>

/* destroys the message when everyone has had a copy of it */

static void
__minimal_destroy_message(void* _msg)
{
	struct msg* msg = (struct msg *) _msg;

	free(msg->payload);
	msg->payload = NULL;
	msg->len = 0;
}

int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	struct per_session_data__minimal *pss = (struct per_session_data__minimal *)user;
	struct per_vhost_data__minimal *vhd = (struct per_vhost_data__minimal *) lws_protocol_vh_priv_get(lws_get_vhost(wsi), lws_get_protocol(wsi));
	const struct msg *pmsg;
	struct msg amsg;
	char textBuffer[32];
	int n, m;

	auto* webServer = (WebServer*) lws_context_user(lws_get_context(wsi));

	//Todo Understand
    uint8_t buf[LWS_PRE + 2048], *start = &buf[LWS_PRE], *p = start, *end = &buf[sizeof(buf) - LWS_PRE - 1];

	switch (reason) {
	case LWS_CALLBACK_PROTOCOL_INIT:
		{
			vhd = (per_vhost_data__minimal *)lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
					lws_get_protocol(wsi),
					sizeof(struct per_vhost_data__minimal));
			vhd->context = lws_get_context(wsi);
			vhd->protocol = lws_get_protocol(wsi);
			vhd->vhost = lws_get_vhost(wsi);
			
			vhd->ring = lws_ring_create(sizeof(struct msg), 8, __minimal_destroy_message);
			if (!vhd->ring){
				return 1;
			}
			webServer->WebSocketInitDone(vhd);
		}
		break;

	case LWS_CALLBACK_PROTOCOL_DESTROY:
		lws_ring_destroy(vhd->ring);
		break;

	case LWS_CALLBACK_ESTABLISHED:
		{
			auto connectOk = false;
			auto size = lws_hdr_custom_length(wsi, "X-API-KEY", 9);
			std::string apiKey;

			if(size > 0 ) {
				
				auto value = new char[size+1];
				int z = lws_hdr_custom_copy(wsi, value, size + 1, "X-API-KEY", 9);
				if (z>0) {
					apiKey = std::string(value, size);
				} 
				delete [] value;
			} else {
				//There is an Bug in libwebsocket the Cookie Value must be lager then Cookie name
				size_t max = 32;
				if(lws_http_cookie_get(wsi, "X-API-KEY", textBuffer, &max) == 0) {
					apiKey = std::string(textBuffer, max);
				} else {
					//LOG(WARNING) << "X-API-KEY not found";
				}

			}

			if(!apiKey.empty()) {
				connectOk = webServer->IsApiKeyOk(apiKey);
			}

			if(!connectOk) {
				if (lws_add_http_common_headers(wsi, HTTP_STATUS_UNAUTHORIZED, "", 0, /* no content len */ &p, end))
				{
					return 1;
				}

				if (lws_finalize_write_http_header(wsi, start, &p, end))
				{
					return 1;
				}
				return 0;
			}

			pss->tail = lws_ring_get_oldest_tail(vhd->ring);
			pss->wsi = wsi;
			if (lws_hdr_copy(wsi, textBuffer, sizeof(textBuffer), WSI_TOKEN_GET_URI) > 0)
				pss->publishing = !strcmp(textBuffer, "/publisher");
			if (!pss->publishing) {
				/* add subscribers to the list of live pss held in the vhd */
				lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
			}
			uuid_generate_time_safe(pss->connectionId);

			webServer->NewWebSocketClient(!pss->publishing, pss->connectionId);
		}
		break;

	case LWS_CALLBACK_CLOSED:
		/* remove our closing pss from the list of live pss */
		lws_ll_fwd_remove(struct per_session_data__minimal, pss_list,
				  pss, vhd->pss_list);
		webServer->RemoveWebSocketClient(!pss->publishing, pss->connectionId);
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:

		if (pss->publishing)
			break;

		pmsg = (msg*)(lws_ring_get_element(vhd->ring, &pss->tail));
		if (!pmsg)
			break;

		/* notice we allowed for LWS_PRE in the payload already */
		m = lws_write(wsi, ((unsigned char *)pmsg->payload) + LWS_PRE,
			      pmsg->len, LWS_WRITE_TEXT);
		if (m < (int)pmsg->len) {
			lwsl_err("ERROR %d writing to ws socket\n", m);
			return -1;
		}

		lws_ring_consume_and_update_oldest_tail(
			vhd->ring,	/* lws_ring object */
			struct per_session_data__minimal, /* type of objects with tails */
			&pss->tail,	/* tail of guy doing the consuming */
			1,		/* number of payload objects being consumed */
			vhd->pss_list,	/* head of list of objects with tails */
			tail,		/* member name of tail in objects with tails */
			pss_list	/* member name of next object in objects with tails */
		);

		/* more to do? */
		if (lws_ring_get_element(vhd->ring, &pss->tail))
			/* come back as soon as we can write more */
			lws_callback_on_writable(pss->wsi);
		break;

	case LWS_CALLBACK_RECEIVE:

		webServer->WebSocketClientMessage(std::string((char*) in, len), pss->connectionId);

		//on publishinger we send to all
		if (!pss->publishing)
			break;

		/*
		 * For test, our policy is ignore publishing when there are
		 * no subscribers connected.
		 */
		if (!vhd->pss_list)
			break;

		n = (int)lws_ring_get_count_free_elements(vhd->ring);
		if (!n) {
			lwsl_user("dropping!\n");
			break;
		}

		amsg.len = len;
		/* notice we over-allocate by LWS_PRE */
		amsg.payload = malloc(LWS_PRE + len);
		if (!amsg.payload) {
			lwsl_user("OOM: dropping\n");
			break;
		}

		memcpy((char *)amsg.payload + LWS_PRE, in, len);
		if (!lws_ring_insert(vhd->ring, &amsg, 1)) {
			__minimal_destroy_message(&amsg);
			lwsl_user("dropping 2!\n");
			break;
		}

		/*
		 * let every subscriber know we want to write something
		 * on them as soon as they are ready
		 */
		lws_start_foreach_llp(struct per_session_data__minimal **,
				      ppss, vhd->pss_list) {
			if (!(*ppss)->publishing)
				lws_callback_on_writable((*ppss)->wsi);
		} lws_end_foreach_llp(ppss, pss_list);
		break;

	default:
		break;
	}

	return 0;
}