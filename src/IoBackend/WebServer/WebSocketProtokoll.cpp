#include "WebSocketProtokoll.hpp"

/* destroys the message when everyone has had a copy of it */

static void
__minimal_destroy_message(void* _msg)
{
	struct msg* msg = (struct msg *) _msg;

	free(msg->payload);
	msg->payload = NULL;
	msg->len = 0;
}

int
callback_websocket(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	struct per_session_data__minimal *pss =
			(struct per_session_data__minimal *)user;
	struct per_vhost_data__minimal *vhd =
			(struct per_vhost_data__minimal *)
			lws_protocol_vh_priv_get(lws_get_vhost(wsi),
					lws_get_protocol(wsi));
	const struct msg *pmsg;
	struct msg amsg;
	char buf[32];
	int n, m;

	switch (reason) {
	case LWS_CALLBACK_PROTOCOL_INIT:
		vhd = (per_vhost_data__minimal *)lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
				lws_get_protocol(wsi),
				sizeof(struct per_vhost_data__minimal));
		vhd->context = lws_get_context(wsi);
		vhd->protocol = lws_get_protocol(wsi);
		vhd->vhost = lws_get_vhost(wsi);

		vhd->ring = lws_ring_create(sizeof(struct msg), 8,
					    __minimal_destroy_message);
		if (!vhd->ring)
			return 1;
		break;

	case LWS_CALLBACK_PROTOCOL_DESTROY:
		lws_ring_destroy(vhd->ring);
		break;

	case LWS_CALLBACK_ESTABLISHED:
		pss->tail = lws_ring_get_oldest_tail(vhd->ring);
		pss->wsi = wsi;
		if (lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_GET_URI) > 0)
			pss->publishing = !strcmp(buf, "/publisher");
		if (!pss->publishing)
			/* add subscribers to the list of live pss held in the vhd */
			lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
		break;

	case LWS_CALLBACK_CLOSED:
		/* remove our closing pss from the list of live pss */
		lws_ll_fwd_remove(struct per_session_data__minimal, pss_list,
				  pss, vhd->pss_list);
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