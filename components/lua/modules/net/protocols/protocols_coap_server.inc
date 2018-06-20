/**
 * Author: Alex Moroz; https://github.com/AlexMoroz
 * 
 * Lua RTOS CoAP server module
 * 
 * Based on esp-idf CoAP server example
 **/

#include "sdkconfig.h"

//#if CONFIG_LUA_RTOS_LUA_USE_PROTOCOLS

#include <string.h>
#include <sys/socket.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_event_loop.h"

#include "nvs_flash.h"

#include "coap.h"

#define COAP_DEFAULT_TIME_SEC 5
#define COAP_DEFAULT_TIME_USEC 0

static TaskHandle_t lcoap_task = NULL;

const static char *TAG = "CoAP_server";

static coap_async_state_t *async = NULL;

static void
send_async_response(coap_context_t *ctx, const coap_endpoint_t *local_if)
{
    coap_pdu_t *response;
    unsigned char buf[3];
    const char* response_data     = "Hello World!!!!";
    size_t size = sizeof(coap_hdr_t) + 20;
    response = coap_pdu_init(async->flags & COAP_MESSAGE_CON, COAP_RESPONSE_CODE(205), 0, size);
    response->hdr->id = coap_new_message_id(ctx);
    if (async->tokenlen)
        coap_add_token(response, async->tokenlen, async->token);
    coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
    coap_add_data  (response, strlen(response_data), (unsigned char *)response_data);

    if (coap_send(ctx, local_if, &async->peer, response) == COAP_INVALID_TID) {

    }
    coap_delete_pdu(response);
    coap_async_state_t *tmp;
    coap_remove_async(ctx, async->id, &tmp);
    coap_free_async(async);
    async = NULL;
}

/*
 * The resource handler
 */
static void
async_handler(coap_context_t *ctx, struct coap_resource_t *resource,
              const coap_endpoint_t *local_interface, coap_address_t *peer,
              coap_pdu_t *request, str *token, coap_pdu_t *response)
{
    async = coap_register_async(ctx, peer, request, COAP_ASYNC_SEPARATE | COAP_ASYNC_CONFIRM, (void*)"no data");
}

static void coap_thread() {
    coap_context_t*  ctx = NULL;
    coap_address_t   serv_addr;
    coap_resource_t* resource = NULL;
    fd_set           readfds;
    struct timeval tv;
    int flags = 0;

    while (1) {
        coap_address_init(&serv_addr);
        serv_addr.addr.sin.sin_family      = AF_INET;
        serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
        serv_addr.addr.sin.sin_port        = htons(COAP_DEFAULT_PORT);
        ctx                                = coap_new_context(&serv_addr);
        if (ctx) {
            flags = fcntl(ctx->sockfd, F_GETFL, 0);
            fcntl(ctx->sockfd, F_SETFL, flags|O_NONBLOCK);

            tv.tv_usec = COAP_DEFAULT_TIME_USEC;
            tv.tv_sec = COAP_DEFAULT_TIME_SEC;
            /* Initialize the resource */
            resource = coap_resource_init((unsigned char *)"coap", 4, 0);
            if (resource){
                coap_register_handler(resource, COAP_REQUEST_GET, async_handler);
                coap_add_resource(ctx, resource);
                /*For incoming connections*/
                for (;;) {
                    FD_ZERO(&readfds);
                    FD_CLR( ctx->sockfd, &readfds);
                    FD_SET( ctx->sockfd, &readfds);

                    int result = select( ctx->sockfd+1, &readfds, 0, 0, &tv );
                    if (result > 0){
                        if (FD_ISSET( ctx->sockfd, &readfds ))
                            coap_read(ctx);
                    } else if (result < 0){
                        break;
                    } else {
                        // do nothing, waiting for client...
                        ESP_LOGE(TAG, "dddd");
                    }

                    if (async) {
                        send_async_response(ctx, ctx->endpoint);
                    }
                }
            }

            coap_free_context(ctx);
        }
    }

    vTaskDelete(NULL);
}

static int lcoap_start(lua_State *L) {



    BaseType_t ret = xTaskCreatePinnedToCore(coap_thread, "lcoap", CONFIG_LUA_RTOS_LUA_THREAD_STACK_SIZE, NULL, CONFIG_LUA_RTOS_LUA_THREAD_PRIORITY, &lcoap_task, xPortGetCoreID());
    assert(ret == pdPASS);
    return 0;
}

static int lcoap_stop(lua_State *L) {
    vTaskDelete(lcoap_task);
    return 0;
}

static const LUA_REG_TYPE lcoap_map[] = {
	{ LSTRKEY("start"), LFUNCVAL(lcoap_start) },
    { LSTRKEY("stop"), LFUNCVAL(lcoap_stop) },
	{ LNILKEY,LNILVAL }
};


//#endif