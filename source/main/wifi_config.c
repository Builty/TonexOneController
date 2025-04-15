/*
 Copyright (C) 2024  Greg Smith

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 
*/


#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_check.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_ota_ops.h"
#include "sys/param.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "json_parser.h"
#include "json_generator.h"
#include "mdns.h"
#include <esp_http_server.h>
#include "control.h"
#include "wifi_config.h"
#include "usb_comms.h"
#include "task_priorities.h"
#include "tonex_params.h"

#define WIFI_CONFIG_TASK_STACK_SIZE   (3 * 1024)

#define ESP_WIFI_SSID           "TonexConfig"
#define ESP_WIFI_PASS           "12345678"
#define ESP_WIFI_CHANNEL        7
#define MAX_STA_CONN            2
#define WIFI_STA_MAXIMUM_RETRY  5
#define WIFI_CONNECTED_BIT      BIT0
#define WIFI_FAIL_BIT           BIT1
#define MAX_TEMP_BUFFER         (20 * 1024)
#define MAX_TEXT_LENGTH         128

static int s_retry_num = 0;
static int wifi_connect_status = 0;
static const char *TAG = "wifi_config";
static uint8_t client_connected = 0;
static httpd_handle_t http_server = NULL;
static httpd_config_t http_config = HTTPD_DEFAULT_CONFIG();
static EventGroupHandle_t s_wifi_event_group;
static QueueHandle_t wifi_input_queue;

static esp_err_t index_get_handler(httpd_req_t *req);
static esp_err_t get_handler(httpd_req_t *req);
static esp_err_t ws_handler(httpd_req_t *req);
static void wifi_kill_all(void);
static void wifi_build_params_json(void);
static void wifi_build_config_json(void);
static void wifi_build_preset_json(void);

enum WiFivents
{
    EVENT_SYNC_PARAMS,
    EVENT_SYNC_PRESET,
    EVENT_SYNC_CONFIG
};

typedef struct
{
    uint8_t Event;
    uint32_t Value;
    char Text[MAX_TEXT_LENGTH];
} tWiFiMessage;

typedef struct
{
    httpd_handle_t hd;
    int fd;
} async_resp_arg;

typedef struct 
{    
    jparse_ctx_t jctx;
    json_gen_str_t jstr;
    httpd_ws_frame_t ws_rsp;
    char PresetName[MAX_TEXT_LENGTH];
    uint16_t PresetIndex;
    uint8_t ParamsChanged : 1;
    uint8_t PresetChanged : 1;
    uint8_t ConfigChanged : 1;
    char wifi_ssid[MAX_WIFI_SSID_PW];
    char wifi_password[MAX_WIFI_SSID_PW];
    char TempBuffer[MAX_TEMP_BUFFER];
} tWebConfigData;

static const httpd_uri_t index_get = 
{
	.uri	  = "/",
	.method   = HTTP_GET,
	.handler  = index_get_handler,
	.user_ctx = NULL
};

static const httpd_uri_t ws = {
    .uri        = "/ws",
    .method     = HTTP_GET,
    .handler    = ws_handler,
    .user_ctx   = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = true
};

// web page for config
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

static esp_err_t stop_webserver(void);
static void wifi_init_sta(void);
static tWebConfigData* pWebConfig;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint8_t process_wifi_command(tWiFiMessage* message)
{
    ESP_LOGI(TAG, "command %d", message->Event);

    // check what we got
    switch (message->Event)
    {
        case EVENT_SYNC_PARAMS:
        {
            // send to all web sockets clients
            //ws_send_all_clients(&http_server, &send_params_async);

            pWebConfig->ParamsChanged = 1;
        } break;

        case EVENT_SYNC_PRESET:
        {
            // save preset details
            memcpy((void*)pWebConfig->PresetName, (void*)message->Text, MAX_TEXT_LENGTH - 1);
            pWebConfig->PresetIndex = message->Value;

            // send to all web sockets clients
            //ws_send_all_clients(&http_server, &send_preset_async);

            pWebConfig->PresetChanged = 1;
        } break;

        case EVENT_SYNC_CONFIG:
        {
            // send to all web sockets clients
            //ws_send_all_clients(&http_server, &send_config_async);
            
            pWebConfig->ConfigChanged = 1;
        } break;    
    }

    return 1;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void wifi_request_sync(uint8_t type, void* arg1, void* arg2)
{
    tWiFiMessage message;

    ESP_LOGI(TAG, "wifi_request_sync");            

    switch (type)
    {
        case  WIFI_SYNC_TYPE_PARAMS:
        default:
        {
            message.Event = EVENT_SYNC_PARAMS;
        } break;

        case WIFI_SYNC_TYPE_PRESET:
        {
            message.Event = EVENT_SYNC_PRESET;

            // get preset name
            memcpy((void*)message.Text, arg1, MAX_TEXT_LENGTH - 1);

            // get preset index
            message.Value = *(uint32_t*)arg2;
        } break;

        case WIFI_SYNC_TYPE_CONFIG:
        {
            message.Event = EVENT_SYNC_CONFIG;
        } break;
    }

    // send to queue
    if (xQueueSend(wifi_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "wifi_request_sync queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static esp_err_t build_send_ws_response_packet(httpd_req_t *req, char* payload)
{
    esp_err_t ret;

    // clear anyt old data
    memset(&pWebConfig->ws_rsp, 0, sizeof(httpd_ws_frame_t));

    pWebConfig->ws_rsp.type = HTTPD_WS_TYPE_TEXT;
    pWebConfig->ws_rsp.payload = (uint8_t*)payload;
    pWebConfig->ws_rsp.len = strlen(payload);
    
    // send it
    ret = httpd_ws_send_frame(req, &pWebConfig->ws_rsp);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }        

    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static void wifi_build_params_json(void)
{
    char str_val[64];
    tTonexParameter* param_ptr;

    // init generation of json response
    json_gen_str_start(&pWebConfig->jstr, pWebConfig->TempBuffer, MAX_TEMP_BUFFER, NULL, NULL);

    // start json object, adds {
    json_gen_start_object(&pWebConfig->jstr);

    // add response
    json_gen_obj_set_string(&pWebConfig->jstr, "CMD", "GETPARAMS");

    json_gen_push_object(&pWebConfig->jstr, "PARAMS");

    for (uint16_t loop = 0; loop < TONEX_GLOBAL_LAST; loop++)
    {
        // get access to parameters
        tonex_params_get_locked_access(&param_ptr);

        // add param index
        sprintf(str_val, "%d", loop);
        json_gen_push_object(&pWebConfig->jstr, str_val);

        // add param details
        json_gen_obj_set_float(&pWebConfig->jstr, "Val", param_ptr[loop].Value);
        json_gen_obj_set_float(&pWebConfig->jstr, "Min", param_ptr[loop].Min);
        json_gen_obj_set_float(&pWebConfig->jstr, "Max", param_ptr[loop].Max);
        json_gen_obj_set_string(&pWebConfig->jstr, "NAME", param_ptr[loop].Name);

        json_gen_pop_object(&pWebConfig->jstr);

        // don't hog the param pointer                    
        tonex_params_release_locked_access();
    }
    
    // add the } for PARAMS
    json_gen_pop_object(&pWebConfig->jstr);

    // add the } for end
    json_gen_end_object(&pWebConfig->jstr);

    // end generation
    json_gen_str_end(&pWebConfig->jstr);

    //debug ESP_LOGI(TAG, "Json: %s", pWebConfig->TempBuffer);

}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static void wifi_build_config_json(void)
{
    char str_val[64];

    // init generation of json response
    json_gen_str_start(&pWebConfig->jstr, pWebConfig->TempBuffer, MAX_TEMP_BUFFER, NULL, NULL);

    // start json object, adds {
    json_gen_start_object(&pWebConfig->jstr);

    // add response
    json_gen_obj_set_string(&pWebConfig->jstr, "CMD", "GETCONFIG");

    // add config
    json_gen_obj_set_int(&pWebConfig->jstr, "BT_MODE", control_get_config_item_int(CONFIG_ITEM_BT_MODE));
    json_gen_obj_set_int(&pWebConfig->jstr, "BT_CHOC_EN", control_get_config_item_int(CONFIG_ITEM_MV_CHOC_ENABLE));
    json_gen_obj_set_int(&pWebConfig->jstr, "BT_MD1_EN", control_get_config_item_int(CONFIG_ITEM_XV_MD1_ENABLE));
    json_gen_obj_set_int(&pWebConfig->jstr, "BT_CUST_EN", control_get_config_item_int(CONFIG_ITEM_CUSTOM_BT_ENABLE));

    control_get_config_item_string(CONFIG_ITEM_BT_CUSTOM_NAME, str_val);
    json_gen_obj_set_string(&pWebConfig->jstr, "BT_CUST_NAME", str_val);

    json_gen_obj_set_int(&pWebConfig->jstr, "TOGGLE_BYPASS", control_get_config_item_int(CONFIG_ITEM_TOGGLE_BYPASS));
    json_gen_obj_set_int(&pWebConfig->jstr, "S_MIDI_EN", control_get_config_item_int(CONFIG_ITEM_MIDI_ENABLE));
    json_gen_obj_set_int(&pWebConfig->jstr, "S_MIDI_CH", control_get_config_item_int(CONFIG_ITEM_MIDI_CHANNEL));
    json_gen_obj_set_int(&pWebConfig->jstr, "FOOTSW_MODE", control_get_config_item_int(CONFIG_ITEM_FOOTSWITCH_MODE));
    json_gen_obj_set_int(&pWebConfig->jstr, "BT_MIDI_CC", control_get_config_item_int(CONFIG_ITEM_ENABLE_BT_MIDI_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "WIFI_MODE", control_get_config_item_int(CONFIG_ITEM_WIFI_MODE));
    json_gen_obj_set_int(&pWebConfig->jstr, "WIFI_POWER", control_get_config_item_int(CONFIG_ITEM_WIFI_TX_POWER));
    json_gen_obj_set_int(&pWebConfig->jstr, "SCREEN_ROT", control_get_config_item_int(CONFIG_ITEM_SCREEN_ROTATION));

    control_get_config_item_string(CONFIG_ITEM_WIFI_SSID, str_val);
    json_gen_obj_set_string(&pWebConfig->jstr, "WIFI_SSID", str_val);

    control_get_config_item_string(CONFIG_ITEM_WIFI_PASSWORD, str_val);
    json_gen_obj_set_string(&pWebConfig->jstr, "WIFI_PW", str_val);

    control_get_config_item_string(CONFIG_ITEM_MDNS_NAME, str_val);
    json_gen_obj_set_string(&pWebConfig->jstr, "MDNS_NAME", str_val);

    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_PS_LAYOUT", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_PRESET_LAYOUT));

    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES1_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES1_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES1_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES1_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL2));
    
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES2_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES2_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES2_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES2_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL2));
    
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES3_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES3_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES3_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES3_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL2));
    
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES4_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES4_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES4_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES4_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL2));
    
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES5_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_SW));    
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES5_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES5_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES5_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL2));
    
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES6_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES6_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES6_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES6_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL2));

    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES7_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES7_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES7_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES7_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL2));

    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES8_SW", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES8_CC", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES8_V1", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "EXTFS_ES8_V2", control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL2));

    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES1_SW", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES1_CC", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES1_V1", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES1_V2", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL2));

    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES2_SW", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES2_CC", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES2_V1", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES2_V2", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL2));

    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES3_SW", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES3_CC", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES3_V1", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES3_V2", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL2));

    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES4_SW", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_SW));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES4_CC", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_CC));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES4_V1", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL1));
    json_gen_obj_set_int(&pWebConfig->jstr, "INTFS_ES4_V2", control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL2));

    // add the }
    json_gen_end_object(&pWebConfig->jstr);

    // end generation
    json_gen_str_end(&pWebConfig->jstr);

    //debug ESP_LOGI(TAG, "Json: %s", pWebConfig->TempBuffer);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static void wifi_build_preset_json(void)
{
    // init generation of json response
    json_gen_str_start(&pWebConfig->jstr, pWebConfig->TempBuffer, MAX_TEMP_BUFFER, NULL, NULL);

    // start json object, adds {
    json_gen_start_object(&pWebConfig->jstr);

    // add response
    json_gen_obj_set_string(&pWebConfig->jstr, "CMD", "GETPRESET");

    // add preset details
    json_gen_obj_set_string(&pWebConfig->jstr, "NAME", pWebConfig->PresetName);
    json_gen_obj_set_int(&pWebConfig->jstr, "INDEX", pWebConfig->PresetIndex);

    // add the }
    json_gen_end_object(&pWebConfig->jstr);

    // end generation
    json_gen_str_end(&pWebConfig->jstr);

    //debug ESP_LOGI(TAG, "Json: %s", pWebConfig->TempBuffer);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "New client connected %d", sockfd);
    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
void wss_close_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "Client disconnected %d", sockfd);
    close(sockfd);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static esp_err_t ws_handler(httpd_req_t *req)
{
    httpd_ws_frame_t ws_pkt;
    uint8_t* buf = NULL;
    char str_val[64];
    int int_val;

    if (req->method == HTTP_GET) 
    {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    
    // Set max_len = 0 to get the frame len 
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    
    //debug ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) 
    {
        // ws_pkt.len + 1 is for NULL termination as we are expecting a string 
        buf = heap_caps_malloc(ws_pkt.len + 1, MALLOC_CAP_SPIRAM);
        if (buf == NULL) 
        {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        memset((void*)buf, 0, ws_pkt.len + 1);
        
        ws_pkt.payload = buf;
        
        // Set max_len = ws_pkt.len to get the frame payload
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) 
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        
        //ESP_LOGI(TAG, "Got ws packet with message: %s", ws_pkt.payload);

        if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE)
        {
            // Response CLOSE packet with no payload to peer
            ESP_LOGI(TAG, "Client Close");

            ws_pkt.len = 0;
            ws_pkt.payload = NULL;

            ret = httpd_ws_send_frame(req, &ws_pkt);
        }
        else if (ws_pkt.type == HTTPD_WS_TYPE_TEXT)
        {
            // parse the json command        
            //debug ESP_LOGI(TAG, "%s", ws_pkt.payload);

            if (json_parse_start(&pWebConfig->jctx, (const char*)ws_pkt.payload, strlen((const char*)ws_pkt.payload)) == OS_SUCCESS)
            {
                // get the command
                if (json_obj_get_string(&pWebConfig->jctx, "CMD", str_val, sizeof(str_val)) == OS_SUCCESS)
                {
                    //debug ESP_LOGI(TAG, "WS got command %s", str_val);

                    if (strcmp(str_val, "GETPARAMS") == 0)
                    {
                        // send current params
                        ESP_LOGI(TAG, "Param request");

                        // build json
                        wifi_build_params_json();   
                        
                        // build packet and send
                        build_send_ws_response_packet(req, pWebConfig->TempBuffer);              
                    }
                    else if (strcmp(str_val, "GETCONFIG") == 0)
                    {
                        // send current config
                        ESP_LOGI(TAG, "Config request");

                        // build response
                        wifi_build_config_json();
                        
                        // build packet and send
                        build_send_ws_response_packet(req, pWebConfig->TempBuffer);
                    }
                    else if (strcmp(str_val, "GETPRESET") == 0)
                    {
                        // send current preset details
                        ESP_LOGI(TAG, "Preset request");

                        // build json response
                        wifi_build_preset_json();
                        
                        // build packet and send
                        build_send_ws_response_packet(req, pWebConfig->TempBuffer);
                    }
                    else if (strcmp(str_val, "SETCONFIG") == 0)
                    {
                        // set config
                        ESP_LOGI(TAG, "Config Set");

                        if (json_obj_get_int(&pWebConfig->jctx, "S_MIDI_EN", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_MIDI_ENABLE, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "S_MIDI_CH", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_MIDI_CHANNEL, int_val);        
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "TOGGLE_BYPASS", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_TOGGLE_BYPASS, int_val);
                        }
                        
                        if (json_obj_get_int(&pWebConfig->jctx, "BT_MODE", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_BT_MODE, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "BT_CHOC_EN", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_MV_CHOC_ENABLE, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "BT_MD1_EN", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_XV_MD1_ENABLE, int_val);
                        }

                        // pause a little to allow control task a chance to process    
                        vTaskDelay(pdMS_TO_TICKS(250));    

                        if (json_obj_get_int(&pWebConfig->jctx, "BT_CUST_EN", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_CUSTOM_BT_ENABLE, int_val);
                        }
                        
                        if (json_obj_get_string(&pWebConfig->jctx, "BT_CUST_NAME", str_val, sizeof(str_val)) == OS_SUCCESS)
                        {
                            control_set_config_item_string(CONFIG_ITEM_BT_CUSTOM_NAME, str_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "BT_MIDI_CC", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_ENABLE_BT_MIDI_CC, int_val);
                        }
                        
                        if (json_obj_get_int(&pWebConfig->jctx, "FOOTSW_MODE", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_FOOTSWITCH_MODE, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "SCREEN_ROT", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_SCREEN_ROTATION, int_val);
                        }
                                    
                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_PS_LAYOUT", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_PRESET_LAYOUT, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES1_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES1_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES1_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL1, int_val);
                        }

                        // pause a little to allow control task a chance to process    
                        vTaskDelay(pdMS_TO_TICKS(250));    

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES1_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES2_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx,  "EXTFS_ES2_CC", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES2_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES2_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES3_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES3_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES3_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES3_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL2, int_val);
                        }

                        // pause a little to allow control task a chance to process    
                        vTaskDelay(pdMS_TO_TICKS(250));    

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES4_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES4_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES4_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES4_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES5_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES5_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES5_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES5_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES6_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES6_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES6_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES6_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES7_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES7_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES7_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES7_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES8_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES8_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES8_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "EXTFS_ES8_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES1_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES1_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES1_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES1_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL2, int_val);
                        }

                        // pause a little to allow control task a chance to process    
                        vTaskDelay(pdMS_TO_TICKS(250));   

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES2_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES2_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES2_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES2_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES3_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES3_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES3_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES3_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL2, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES4_SW", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_SW, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES4_CC", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_CC, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES4_V1", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL1, int_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "INTFS_ES4_V2", &int_val) == OS_SUCCESS) 
                        {
                            control_set_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL2, int_val);
                        }

                        vTaskDelay(pdMS_TO_TICKS(250));

                        // save it and reboot after
                        control_save_user_data(1);
                    }
                    else if (strcmp(str_val, "SETWIFI") == 0)
                    {
                        // set config
                        ESP_LOGI(TAG, "WiFi Set");

                        if (json_obj_get_int(&pWebConfig->jctx, "WIFI_MODE", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_WIFI_MODE, int_val);
                        }

                        if (json_obj_get_string(&pWebConfig->jctx, "WIFI_SSID", str_val, sizeof(str_val)) == OS_SUCCESS)
                        {
                            control_set_config_item_string(CONFIG_ITEM_WIFI_SSID, str_val);
                        }

                        if (json_obj_get_string(&pWebConfig->jctx, "WIFI_PW", str_val, sizeof(str_val)) == OS_SUCCESS)
                        {
                            control_set_config_item_string(CONFIG_ITEM_WIFI_PASSWORD, str_val);
                        }

                        if (json_obj_get_int(&pWebConfig->jctx, "WIFI_POWER", &int_val) == OS_SUCCESS)
                        {
                            control_set_config_item_int(CONFIG_ITEM_WIFI_TX_POWER, int_val);
                        }

                        if (json_obj_get_string(&pWebConfig->jctx, "MDNS_NAME", str_val, sizeof(str_val)) == OS_SUCCESS)
                        {
                            control_set_config_item_string(CONFIG_ITEM_MDNS_NAME, str_val);
                        }

                        vTaskDelay(pdMS_TO_TICKS(250));

                        // save it and reboot after
                        control_save_user_data(1);
                    }                
                    else if (strcmp(str_val, "SETPARAM") == 0)
                    {
                        int index;
                        float value;

                        ESP_LOGI(TAG, "Set Param");

                        if (json_obj_get_int(&pWebConfig->jctx, "INDEX", &index) == OS_SUCCESS)
                        {
                            if (json_obj_get_float(&pWebConfig->jctx, "VALUE", &value) == OS_SUCCESS)
                            {
                                usb_modify_parameter(index, value);
                            }
                            else
                            {
                                ESP_LOGW(TAG, "Could't find param value");
                            }
                        }
                    }
                    else if (strcmp(str_val, "SETPRESET") == 0)
                    {
                        // set preset
                        ESP_LOGI(TAG, "Preset Set");

                        if (json_obj_get_int(&pWebConfig->jctx, "PRESET", &int_val) == OS_SUCCESS)
                        {
                            control_request_preset_index(int_val);
                        }
                    }
                    else if (strcmp(str_val, "GETCHANGES") == 0)
                    {
                        // check for any changes
                        if (pWebConfig->ParamsChanged)
                        {
                            // send current params
                            ESP_LOGI(TAG, "Param update");

                            // build json
                            wifi_build_params_json();   
                        
                            // build packet and send
                            build_send_ws_response_packet(req, pWebConfig->TempBuffer);             
                            pWebConfig->ParamsChanged = 0;
                        }
    
                        if (pWebConfig->PresetChanged)
                        {
                            // send current preset
                            ESP_LOGI(TAG, "Preset update");

                            // build json response
                            wifi_build_preset_json();
                        
                            // build packet and send
                            build_send_ws_response_packet(req, pWebConfig->TempBuffer);

                            pWebConfig->PresetChanged = 0;
                        }
             
                        if (pWebConfig->ConfigChanged)
                        {
                            // send current config
                            ESP_LOGI(TAG, "Config update");

                            // build response
                            wifi_build_config_json();
                        
                            // build packet and send
                            build_send_ws_response_packet(req, pWebConfig->TempBuffer);

                            pWebConfig->ConfigChanged = 0;
                        }
                    }                    
                }

                json_parse_end(&pWebConfig->jctx);
            }
        }

        free(buf);
    }    
    
    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static esp_err_t __attribute__((unused)) get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "get_handler\n");

    // Send a simple response
    const char resp[] = "URI GET Response";

    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static esp_err_t index_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char*)index_html_start, index_html_end - index_html_start);
	return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static char from_hex(char ch) 
{
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static char* url_decode(char *str) 
{
    char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;
  
    while (*pstr) 
    {
        if (*pstr == '%') 
        {
            if (pstr[1] && pstr[2]) 
            {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        } 
        else if (*pstr == '+') 
        { 
            *pbuf++ = ' ';
        } 
        else 
        {
            *pbuf++ = *pstr;
        }
    
        pstr++;
    }
  
    *pbuf = '\0';
    
    return buf;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
uint8_t get_submitted_value(char* dest, char* ptr)
{
    char tmp_str[128];
    char* tmp_ptr = tmp_str;
    char* decoded;
    
    // copy until end of value
    while ((*ptr != ' ') && (*ptr != 0) && (*ptr != '&'))
    {
        *tmp_ptr = *ptr;
        tmp_ptr++;
        ptr++;
    } 
    
    // ensure null terminated
    *tmp_ptr = 0;
    
    // decode the URL
    decoded = url_decode(tmp_str);
    
    strcpy(dest, decoded);
    free(decoded);
      
    return 1;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static esp_err_t stop_webserver(void)
{
    // Stop the httpd server
    if (http_server != NULL)
    {
        httpd_stop(http_server);
    }

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static esp_err_t http_server_init(void)
{
    ESP_LOGI(TAG, "Http server init");
    
    // set config
    http_config.task_priority      = tskIDLE_PRIORITY + 1;
    http_config.core_id            = 0;
    http_config.stack_size         = (3 * 1024);  
    http_config.server_port        = 80;
    http_config.ctrl_port          = 32768;
    http_config.max_open_sockets   = 6;
    http_config.max_uri_handlers   = 2;
    http_config.max_resp_headers   = 8;
    http_config.backlog_conn       = 1;
    http_config.keep_alive_enable  = true;
    http_config.keep_alive_idle    = 10;     // seconds
    http_config.keep_alive_interval = 10;    // seconds
    http_config.keep_alive_count = 5;
    http_config.open_fn = NULL;
    http_config.close_fn = NULL;
    http_config.uri_match_fn = NULL;
    http_config.lru_purge_enable = true;

	if (httpd_start(&http_server, &http_config) == ESP_OK) 
    {
        if (http_server != NULL)
        {
            ESP_LOGI(TAG, "Http register uri 1");
    	    httpd_register_uri_handler(http_server, &index_get);

            // Registering the ws handler
            ESP_LOGI(TAG, "Registering ws handler");
            httpd_register_uri_handler(http_server, &ws);
        }
	}

    if (http_server == NULL)
    {
        ESP_LOGE(TAG, "Http server init failed!");
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGI(TAG, "Http server init OK");
        return ESP_OK;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) 
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
        client_connected = 1;

        control_set_wifi_status(1);
    } 
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) 
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
        client_connected = 0;
        control_set_wifi_status(0);

        if (control_get_config_item_int(CONFIG_ITEM_WIFI_MODE) == WIFI_MODE_ACCESS_POINT_TIMED)
        {
            ESP_LOGI(TAG, "Wifi config stopping");
            wifi_kill_all();
        }
    }
    else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START))
    {
        esp_wifi_connect();
    }
    else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED))
    {
        if (s_retry_num < WIFI_STA_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        wifi_connect_status = 0;
        control_set_wifi_status(0);
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if ((event_base == IP_EVENT) && (event_id == IP_EVENT_STA_GOT_IP))
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;

        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        wifi_connect_status = 1;
        control_set_wifi_status(1);
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            // Setting a password implies station will connect to all security modes including WEP/WPA.
            // However these modes are deprecated and not advisable to be used. Incase your Access point
            // doesn't support WPA2, these mode can be enabled by commenting below line
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    // get credentials from config
    control_get_config_item_string(CONFIG_ITEM_WIFI_SSID, pWebConfig->wifi_ssid);
    control_get_config_item_string(CONFIG_ITEM_WIFI_PASSWORD, pWebConfig->wifi_password);

    // set SSID and password
    strcpy((char*)wifi_config.sta.ssid, pWebConfig->wifi_ssid);
    strcpy((char*)wifi_config.sta.password, pWebConfig->wifi_password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
    // number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) 
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    // xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
    // happened. 
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s", pWebConfig->wifi_ssid);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", pWebConfig->wifi_ssid);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    vEventGroupDelete(s_wifi_event_group);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .channel = ESP_WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
    };
    
    // get credentials from config
    control_get_config_item_string(CONFIG_ITEM_WIFI_SSID, pWebConfig->wifi_ssid);
    control_get_config_item_string(CONFIG_ITEM_WIFI_PASSWORD, pWebConfig->wifi_password);

    // set SSID and password
    strcpy((char*)wifi_config.ap.ssid, pWebConfig->wifi_ssid);
    wifi_config.ap.ssid_len = strlen((char*)pWebConfig->wifi_ssid),
    strcpy((char*)wifi_config.ap.password, pWebConfig->wifi_password);

    if (wifi_config.ap.ssid_len == 0) 
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s channel:%d", wifi_config.sta.ssid, ESP_WIFI_CHANNEL);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void start_mdns_service()
{
    char mdns_name[MAX_MDNS_NAME];

    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) 
    {
        ESP_LOGE(TAG, "MDNS Init failed: %d\n", err);
        return;
    }

    control_get_config_item_string(CONFIG_ITEM_MDNS_NAME, mdns_name);
    mdns_hostname_set(mdns_name);
    mdns_instance_name_set(mdns_name);
    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void wifi_kill_all(void)
{
    ESP_LOGI(TAG, "Wifi config stopping");
    stop_webserver();
    vTaskDelay(pdMS_TO_TICKS(5000));
    esp_wifi_stop();
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void wifi_config_task(void *arg)
{
    tWiFiMessage message;
    uint32_t tick_timer;
    uint8_t wifi_kill_checked = 0;
    uint8_t wifi_mode = control_get_config_item_int(CONFIG_ITEM_WIFI_MODE);

    ESP_LOGI(TAG, "Wifi config task start");

    // let everything settle and init
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "Wifi config starting");
    s_wifi_event_group = xEventGroupCreate();

    pWebConfig = heap_caps_malloc(sizeof(tWebConfigData), MALLOC_CAP_SPIRAM);
    if (pWebConfig == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate pWebConfig buffer!");
        return;
    }

    // init mem
    memset((void*)pWebConfig, 0, sizeof(tWebConfigData));
    pWebConfig->PresetIndex = 0;
    sprintf(pWebConfig->PresetName, "1");

    // check wifi mode
    switch (wifi_mode)    
    {
        case WIFI_MODE_STATION:
        {
            // conect to AP
            wifi_init_sta();
        } break;

        case WIFI_MODE_ACCESS_POINT_TIMED:    // fall through
        case WIFI_MODE_ACCESS_POINT:          // fall through
        default:
        {
            // start up WiFi access point
            wifi_init_softap();
        } break;
    }

    // set the WiFi TX power. Some platforms seem to have stability issues on max power
    switch (control_get_config_item_int(CONFIG_ITEM_WIFI_TX_POWER))
    {
        case WIFI_TX_POWER_100:        
        {
            esp_wifi_set_max_tx_power(80);
        } break;

        case WIFI_TX_POWER_75:
        {
            esp_wifi_set_max_tx_power(66);
        } break;

        case WIFI_TX_POWER_50:
        {
            esp_wifi_set_max_tx_power(52);
        } break;

        case WIFI_TX_POWER_25:
        default:
        {
            esp_wifi_set_max_tx_power(28);
        } break;
    }

    int8_t power;
    esp_wifi_get_max_tx_power(&power);
    ESP_LOGI(TAG, "WiFi Tx power: %d dbm", (int)(power * 0.25f));

    start_mdns_service();

    // start web server
    http_server_init();

    // reset timer
    tick_timer = xTaskGetTickCount();

    while (1) 
    {
        if (wifi_mode == WIFI_MODE_ACCESS_POINT_TIMED)
        {            
            if (wifi_kill_checked == 0)
            {
                // allow WiFi AP to run for 60 seconds
                if ((xTaskGetTickCount() - tick_timer) >= 60000)
                {
                    wifi_kill_checked = 1;

                    // any clients connected?
                    if (client_connected == 0)
                    {
                        // kill
                        ESP_LOGI(TAG, "Wifi config stopping");
                        wifi_kill_all();
                    }
                }
            }
        }

        // check for any input messages
        if (xQueueReceive(wifi_input_queue, (void*)&message, pdMS_TO_TICKS(20)) == pdPASS)
        {
            // process it
            process_wifi_command(&message);
        }

		vTaskDelay(pdMS_TO_TICKS(5));
	}
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void wifi_config_init(void)
{
     // create queue for commands from other threads
    wifi_input_queue = xQueueCreate(5, sizeof(tWiFiMessage));
    if (wifi_input_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create WiFi input queue!");
    }

    xTaskCreatePinnedToCore(wifi_config_task, "WIFI", WIFI_CONFIG_TASK_STACK_SIZE, NULL, WIFI_TASK_PRIORITY, NULL, 0);
}
