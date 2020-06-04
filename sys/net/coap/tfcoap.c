/* Copyright 2019 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @defgroup    net_coap
 * @ingroup     net_coap
 *
 * @{
 * @file        tf_coap
 * @brief       Constrained Application Protocol
 *
 * @author      Locha Mesh Developers <developers@locha.io>
 * @}
 */



#define ENABLE_DEBUG (1)
#include "debug.h"
#include "net/tfcoap.h"
#include "cJSON.h"
#include "xtimer.h"

#define COAP_INBUF_SIZE (256U)

extern int _gnrc_netif_config(int argc, char **argv);

/* must be sorted by path (ASCII order) */
static const coap_resource_t _resources[] = {
    { "/system/info", COAP_GET, stats_handler, NULL },
};

static gcoap_listener_t listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    encode_link,
    NULL
};

static const char *_link_params[] = {
    ";ct=0;rt=\"count\";obs",
    NULL
};


ssize_t encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context) {
    ssize_t res = gcoap_encode_link(resource, buf, maxlen, context);
    if (res > 0) {
        if (_link_params[context->link_pos]
                && (strlen(_link_params[context->link_pos]) < (maxlen - res))) {
            if (buf) {
                memcpy(buf+res, _link_params[context->link_pos],
                       strlen(_link_params[context->link_pos]));
            }
            return res + strlen(_link_params[context->link_pos]);
        }
    }

    return res;
}


ssize_t stats_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx)
{
    uint16_t voltage = 0;
    int16_t avg_current = 0;
    int16_t avg_power = 0;
     size_t free_memory = 0;
    const  char *ap_ssid = "RIOT_AP";


    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "device_name", RIOT_BOARD);
    cJSON_AddStringToObject(root, "device_version", RIOT_VERSION);

    cJSON_AddNumberToObject(root, "voltage", voltage * 1.0);
    cJSON_AddNumberToObject(root, "avg_current", avg_current * 1.0);
    cJSON_AddNumberToObject(root, "avg_power", avg_power * 1.0);
    cJSON_AddNumberToObject(root, "avg_temp", avg_power * 1.0);
    cJSON_AddNumberToObject(root, "free_memory", free_memory * 1.0);

    cJSON* ap_root = cJSON_AddObjectToObject(root, "ap");
    cJSON_AddStringToObject(ap_root, "ssid", ap_ssid);

    // ****  STA is not avaliable in this version  **** //

    // cJSON* sta_root = cJSON_AddObjectToObject(root, "sta");
    // cJSON_AddStringToObject(sta_root, "ssid", sta_ssid);
    // cJSON_AddBoolToObject(sta_root, "enabled", sta_enabled);

    char* payload = cJSON_Print(root);

    (void)ctx;
    gcoap_resp_init(pdu, buf, 223, COAP_CODE_CONTENT);
    // coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
    // size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
   
    
    DEBUG("length is: %d \n", pdu->payload_len);
    DEBUG("length of the payload is: %d \n", strlen(payload));
    /* write the RIOT board name in the response buffer */
     if (pdu->payload_len >= strlen(payload)) {
        memcpy(pdu->payload, payload, strlen(payload)+1);
        return gcoap_finish(pdu, strlen(payload) , COAP_FORMAT_TEXT);
    }
    else {
        puts("gcoap_cli: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }

    return 0;
}

void tf_coat_init(void){

    gcoap_register_listener(&listener);
}