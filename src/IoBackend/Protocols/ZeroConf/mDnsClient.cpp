#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "mDnsClient"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "mDnsClient.hpp"

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/error.h>
#include <avahi-common/malloc.h>
#include <avahi-common/simple-watch.h>

#include "../../../common/easylogging/easylogging++.h"
#include "../../../common/utils/commonutils.h"

// TODO no Good !
static AvahiSimplePoll* simple_poll = nullptr;
static void client_callback(AvahiClient* c, AvahiClientState state, void* userdata)
{
    assert(c);
    /* Called whenever the client or server state changes */

    if(state == AVAHI_CLIENT_FAILURE) {
        fprintf(stderr, "Server connection failure: %s\n", avahi_strerror(avahi_client_errno(c)));
        avahi_simple_poll_quit(simple_poll);
    }
}

static void resolve_callback(
    AvahiServiceResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const char *type,
    const char *domain,
    const char *host_name,
    const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
    AvahiLookupResultFlags flags,
    AVAHI_GCC_UNUSED void* userdata) {

    assert(r);

    /* Called whenever a service has been resolved successfully or timed out */

    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            fprintf(stderr, "(Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", name, type, domain, avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))));
            break;

        case AVAHI_RESOLVER_FOUND: {
            char a[AVAHI_ADDRESS_STR_MAX], *t;

            fprintf(stderr, "Service '%s' of type '%s' in domain '%s':\n", name, type, domain);

            avahi_address_snprint(a, sizeof(a), address);
            t = avahi_string_list_to_string(txt);
            fprintf(stderr,
                    "\t%s:%u (%s)\n"
                    "\tTXT=%s\n"
                    "\tcookie is %u\n"
                    "\tis_local: %i\n"
                    "\tour_own: %i\n"
                    "\twide_area: %i\n"
                    "\tmulticast: %i\n"
                    "\tcached: %i\n",
                    host_name, port, a,
                    t,
                    avahi_string_list_get_service_cookie(txt),
                    !!(flags & AVAHI_LOOKUP_RESULT_LOCAL),
                    !!(flags & AVAHI_LOOKUP_RESULT_OUR_OWN),
                    !!(flags & AVAHI_LOOKUP_RESULT_WIDE_AREA),
                    !!(flags & AVAHI_LOOKUP_RESULT_MULTICAST),
                    !!(flags & AVAHI_LOOKUP_RESULT_CACHED));

            avahi_free(t);
        }
    }

    avahi_service_resolver_free(r);
}

static void browse_callback(
    AvahiServiceBrowser *b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char *name,
    const char *type,
    const char *domain,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata) {

    AvahiClient *c = static_cast<AvahiClient *>(userdata);
    assert(b);

    /* Called whenever a new services becomes available on the LAN or is removed from the LAN */

    switch (event) {
        case AVAHI_BROWSER_FAILURE:

            fprintf(stderr, "(Browser) %s\n", avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b))));
            avahi_simple_poll_quit(simple_poll);
            return;

        case AVAHI_BROWSER_NEW:
            fprintf(stderr, "(Browser) NEW: service '%s' of type '%s' in domain '%s'\n", name, type, domain);

            /* We ignore the returned resolver object. In the callback
               function we free it. If the server is terminated before
               the callback function is called the server will free
               the resolver for us. */

            if (!(avahi_service_resolver_new(c, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, static_cast<AvahiLookupFlags>(0), resolve_callback, c)))
                fprintf(stderr, "Failed to resolve service '%s': %s\n", name, avahi_strerror(avahi_client_errno(c)));

            break;

        case AVAHI_BROWSER_REMOVE:
            fprintf(stderr, "(Browser) REMOVE: service '%s' of type '%s' in domain '%s'\n", name, type, domain);
            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:
            avahi_simple_poll_quit(simple_poll);
            fprintf(stderr, "(Browser) %s type '%s'\n", "ALL_FOR_NOW", type);
            break;
        case AVAHI_BROWSER_CACHE_EXHAUSTED:
            fprintf(stderr, "(Browser) %s\n", "CACHE_EXHAUSTED");
            break;
    }
}

mDnsClient::mDnsClient(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

mDnsClient::~mDnsClient()
{
    if(simple_poll != nullptr) {
        avahi_simple_poll_free(simple_poll);
    }
}

void mDnsClient::ScanNetwork(const std::string serviceType)
{
    // type = "_http._tcp")
    //_esphomelib

    if(simple_poll == nullptr) {
        simple_poll = avahi_simple_poll_new();
        if(simple_poll == nullptr) {
            LOG(ERROR) << "Failed to create simple poll object";
            return;
        }
    }

    int error;

    /* Allocate a new client do not faisl wenn server not running*/
    auto client = avahi_client_new(avahi_simple_poll_get(simple_poll), AvahiClientFlags::AVAHI_CLIENT_NO_FAIL,
                                   client_callback, NULL, &error);

    /* Check whether creating the client object succeeded */
    if(!client) {
        LOG(ERROR) << "Failed to create client: " << avahi_strerror(error);
        return;
    }

    auto sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, serviceType.c_str(), NULL, static_cast<AvahiLookupFlags>(0), browse_callback, client);
    /* Create the service browser */
    if(!sb) {
        LOG(ERROR) << "Failed to create service browser: " << avahi_strerror(avahi_client_errno(client));
        avahi_client_free(client);
        return;
    }

    avahi_simple_poll_loop(simple_poll);

    if(sb) avahi_service_browser_free(sb);
    if(client) avahi_client_free(client);
    if(simple_poll) 
    {
        avahi_simple_poll_free(simple_poll);
        simple_poll = nullptr;
    }
}