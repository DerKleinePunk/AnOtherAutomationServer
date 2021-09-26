#pragma once
#include <libwebsockets.h>


/* one of these created for each message */

struct msg {
	void *payload; /* is malloc'd */
	size_t len;
};

/* one of these is created for each client connecting to us */

struct per_session_data__minimal {
	struct per_session_data__minimal *pss_list;
	struct lws *wsi;
	uint32_t tail;
	char publishing; /* nonzero: peer is publishing to us */
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;

	struct per_session_data__minimal *pss_list; /* linked-list of live pss*/

	struct lws_ring *ring; /* ringbuffer holding unsent messages */
};

int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
