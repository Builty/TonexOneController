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

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sys/param.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_device.h"


#include "control.h"
#include "task_priorities.h"
#include "midi_control.h"
#include "midi_helper.h"

static const char *TAG = "MidiBT";
#define GATTC_TAG        "GATTC_CLIENT"
#define GATTS_TAG        "GATTS_SERVER"


// 7772e5db-3868-4112-a1a9-f2669d106bf3   Midi characteristic
static uint8_t MidiCharacteristicUUIDByteReversed[] = {0xF3, 0x6B, 0x10, 0x9D, 0x66, 0xF2, 0xA9, 0xA1, 0x12, 0x41, 0x68, 0x38, 0xDB, 0xE5, 0x72, 0x77};

// 00002a19-0000-1000-8000-00805f9b34fb  battery level
//static uint8_t BatteryLevelCharacteristicUUIDByteReversed[] = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x00, 0x00};

// others in the MVave Chocolate
// 03b80e5a-ede8-4b33-a751-6ce34ec4c700 - Midi service
// 00002a05-0000-1000-8000-00805f9b34fb - service changed
// 00002a4d-0000-1000-8000-00805f9b34fb - report
// 00002a33-0000-1000-8000-00805f9b34fb - boot mouse
// 0000ae42-0000-1000-8000-00805f9b34fb - unknown
// 0000ae02-0000-1000-8000-00805f9b34fb - unknown

#define PROFILE_A_APP_ID            0
#define INVALID_HANDLE              0
#define BT_SCAN_DURATION            (7 * 24 * 60 * 60)   // 7 days, in seconds
#define MAX_DEVICE_NAME_LENGTH      25
#define MAX_DEVICE_NAMES            10

// Declare static functions
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_a_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


static esp_bt_uuid_t remote_filter_char_uuid_reuse[5]; 

static esp_bt_uuid_t notify_descr_uuid = 
{
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
};

static bool conn_device_a   = false;
static bool get_service_a   = false;
static bool Isconnecting    = false;
static bool stop_scan_done  = false;

static uint8_t midi_serial_channel = 0;
static uint16_t search_start_handle = 0xFFFF;
static uint16_t search_end_handle = 0;
static esp_gattc_descr_elem_t *descr_elem_result_a  = NULL;

static char remote_device_names[MAX_DEVICE_NAMES][MAX_DEVICE_NAME_LENGTH];
static uint8_t remote_device_names_length = 0;

static esp_ble_scan_params_t ble_scan_params = 
{
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

struct gattc_profile_inst 
{
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
};

// One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT
static struct gattc_profile_inst gl_profile_tab = 
{
    .gattc_cb = gattc_profile_a_event_handler,
    .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
};



// Server stuff
#define GATTS_NUM_HANDLE_TEST_A     4

static char test_device_name[] = "TnxBT";

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 20
#define PREPARE_BUF_MAX_SIZE        1024

static uint8_t char1_str[20] = {0x00,0x00,0x00};
static esp_gatt_char_prop_t a_property = 0;

static esp_attr_value_t gatts_demo_char1_val =
{
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len     = sizeof(char1_str),
    .attr_value   = char1_str,
};

static uint8_t adv_config_done = 0;
#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

// Midi service UUID
// 03b80e5a-ede8-4b33-a751-6ce34ec4c700
static uint8_t adv_service_uuid128[ESP_UUID_LEN_128] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    0x00, 0xC7, 0xC4, 0x4E, 0xE3, 0x6C, 0x51, 0xA7, 0x33, 0x4B, 0xE8, 0xED, 0x5A, 0x0E, 0xB8, 0x03 
    //0x03, 0xB8, 0x0E, 0x5A, 0xED, 0xE8, 0x4B, 0x33, 0xA7, 0x51, 0x6C, 0xE3, 0x4E, 0xC4, 0xC7, 0x00 
};

//adv data
static esp_ble_adv_data_t adv_data = 
{
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0, 
    .p_manufacturer_data =  NULL, 
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = 
{
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    //.min_interval = 0x0006,
    //.max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0, 
    .p_manufacturer_data =  NULL, 
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst gls_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

typedef struct 
{
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

static prepare_type_env_t a_prepare_write_env;

void server_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void server_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);


/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
    char *key_str = NULL;
   
    switch(key_type) 
    {
        case ESP_LE_KEY_NONE:
            key_str = "ESP_LE_KEY_NONE";
            break;

        case ESP_LE_KEY_PENC:
            key_str = "ESP_LE_KEY_PENC";
            break;

        case ESP_LE_KEY_PID:
            key_str = "ESP_LE_KEY_PID";
            break;

        case ESP_LE_KEY_PCSRK:
            key_str = "ESP_LE_KEY_PCSRK";
            break;

        case ESP_LE_KEY_PLK:
            key_str = "ESP_LE_KEY_PLK";
            break;

        case ESP_LE_KEY_LLK:
            key_str = "ESP_LE_KEY_LLK";
            break;

        case ESP_LE_KEY_LENC:
            key_str = "ESP_LE_KEY_LENC";
            break;

        case ESP_LE_KEY_LID:
            key_str = "ESP_LE_KEY_LID";
            break;

        case ESP_LE_KEY_LCSRK:
            key_str = "ESP_LE_KEY_LCSRK";
            break;

        default:
            key_str = "INVALID BLE KEY TYPE";
            break;
    }

    return key_str;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req)
{
    char *auth_str = NULL;
   
    switch(auth_req) 
    {
        case ESP_LE_AUTH_NO_BOND:
            auth_str = "ESP_LE_AUTH_NO_BOND";
            break;

        case ESP_LE_AUTH_BOND:
            auth_str = "ESP_LE_AUTH_BOND";
            break;

        case ESP_LE_AUTH_REQ_MITM:
            auth_str = "ESP_LE_AUTH_REQ_MITM";
            break;

        case ESP_LE_AUTH_REQ_BOND_MITM:
            auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
            break;

        case ESP_LE_AUTH_REQ_SC_ONLY:
            auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
            break;

        case ESP_LE_AUTH_REQ_SC_BOND:
            auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
            break;

        case ESP_LE_AUTH_REQ_SC_MITM:
            auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
            break;

        case ESP_LE_AUTH_REQ_SC_MITM_BOND:
            auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
            break;

        default:
            auth_str = "INVALID BLE AUTH REQ";
            break;
    }

    return auth_str;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void show_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num == 0) 
    {
        ESP_LOGI(GATTS_TAG, "Bonded devices number zero\n");
        return;
    }

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    if (!dev_list) 
    {
        ESP_LOGI(GATTS_TAG, "malloc failed, return\n");
        return;
    }
    
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    ESP_LOGI(GATTS_TAG, "Bonded devices number %d", dev_num);
    
    for (int i = 0; i < dev_num; i++) 
    {
        //ESP_LOGI(GATTS_TAG, "[%u] addr_type %u, addr "ESP_BD_ADDR_STR"", i, dev_list[i].bd_addr_type, ESP_BD_ADDR_HEX(dev_list[i].bd_addr));
    }

    free(dev_list);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) remove_all_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num == 0) 
    {
        ESP_LOGI(GATTS_TAG, "Bonded devices number zero\n");
        return;
    }

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    if (!dev_list) 
    {
        ESP_LOGI(GATTS_TAG, "malloc failed, return\n");
        return;
    }
    
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    for (int i = 0; i < dev_num; i++) 
    {
        esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }

    free(dev_list);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void server_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    esp_gatt_status_t status = ESP_GATT_OK;
    
    if (param->write.need_rsp)
    {
        if (param->write.is_prep) 
        {
            if (param->write.offset > PREPARE_BUF_MAX_SIZE) 
            {
                status = ESP_GATT_INVALID_OFFSET;
            } 
            else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) 
            {
                status = ESP_GATT_INVALID_ATTR_LEN;
            }
    
            if (status == ESP_GATT_OK && prepare_write_env->prepare_buf == NULL) 
            {
                prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE*sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;
                if (prepare_write_env->prepare_buf == NULL) 
                {
                    ESP_LOGE(GATTS_TAG, "Gatt_server prep no mem");
                    status = ESP_GATT_NO_RESOURCES;
                }
            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            if (gatt_rsp) 
            {
                gatt_rsp->attr_value.len = param->write.len;
                gatt_rsp->attr_value.handle = param->write.handle;
                gatt_rsp->attr_value.offset = param->write.offset;
                gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
                memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
                
                esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
                if (response_err != ESP_OK)
                {
                    ESP_LOGE(GATTS_TAG, "Send response error\n");
                }
                free(gatt_rsp);
            } 
            else 
            {
                ESP_LOGE(GATTS_TAG, "malloc failed, no resource to send response error\n");
                status = ESP_GATT_NO_RESOURCES;
            }
            
            if (status != ESP_GATT_OK)
            {
                return;
            }
            
            memcpy(prepare_write_env->prepare_buf + param->write.offset,
                   param->write.value,
                   param->write.len);
            prepare_write_env->prepare_len += param->write.len;

        }
        else
        {
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void server_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC)
    {
        ESP_LOG_BUFFER_HEX(GATTS_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }
    else
    {
        ESP_LOGI(GATTS_TAG,"Prepare write cancel");
    }
    
    if (prepare_write_env->prepare_buf) 
    {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) 
{
    switch (event) 
    {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "GATT server register, status %d, app_id %d, gatts_if %d", param->reg.status, param->reg.app_id, gatts_if);
        gls_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gls_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
        gls_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_128;
        memcpy((void*)gls_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid128, (void*)adv_service_uuid128, ESP_UUID_LEN_128);

        esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(test_device_name);
        if (set_dev_name_ret)
        {
            ESP_LOGE(GATTS_TAG, "set device name failed, error code = %x", set_dev_name_ret);
        }

        //config adv data
        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret)
        {
            ESP_LOGE(GATTS_TAG, "config adv data failed, error code = %x", ret);
        }
        adv_config_done |= adv_config_flag;
        
        //config scan response data
        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret)
        {
            ESP_LOGE(GATTS_TAG, "config scan response data failed, error code = %x", ret);
        }
        adv_config_done |= scan_rsp_config_flag;

        esp_ble_gatts_create_service(gatts_if, &gls_profile_tab[PROFILE_A_APP_ID].service_id, GATTS_NUM_HANDLE_TEST_A);
        break;

    case ESP_GATTS_READ_EVT: 
    {
        ESP_LOGI(GATTS_TAG, "Characteristic read, conn_id %d, trans_id %" PRIu32 ", handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0xde;
        rsp.attr_value.value[1] = 0xed;
        rsp.attr_value.value[2] = 0xbe;
        rsp.attr_value.value[3] = 0xef;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);
        break;
    }

    case ESP_GATTS_WRITE_EVT: 
    {
        ESP_LOGI(GATTS_TAG, "Characteristic write, conn_id %d, trans_id %" PRIu32 ", handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep)
        {
            ESP_LOGI(GATTS_TAG, "value len %d, value ", param->write.len);
            //ESP_LOG_BUFFER_HEX(GATTS_TAG, param->write.value, param->write.len);

            // handle Midi data.
            midi_helper_process_incoming_data(param->write.value, param->write.len, midi_serial_channel, 1);            

            if (gls_profile_tab[PROFILE_A_APP_ID].descr_handle == param->write.handle && param->write.len == 2)
            {
                uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                if (descr_value == 0x0001)
                {
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY){
                        ESP_LOGI(GATTS_TAG, "Notification enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i%0xff;
                        }
                        //the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gls_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                sizeof(notify_data), notify_data, false);
                    }
                }
                else if (descr_value == 0x0002)
                {
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_INDICATE)
                    {
                        ESP_LOGI(GATTS_TAG, "Indication enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i%0xff;
                        }
                        
                        //the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gls_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                sizeof(indicate_data), indicate_data, true);
                    }
                }
                else if (descr_value == 0x0000)
                {
                    ESP_LOGI(GATTS_TAG, "Notification/Indication disable");
                }
                else
                {
                    ESP_LOGE(GATTS_TAG, "Unknown descriptor value");
                    ESP_LOG_BUFFER_HEX(GATTS_TAG, param->write.value, param->write.len);
                }

            }
        }
        server_write_event_env(gatts_if, &a_prepare_write_env, param);
        break;
    }

    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(GATTS_TAG,"Execute write");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        server_exec_write_event_env(&a_prepare_write_env, param);
        break;

    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_TAG, "MTU exchange, MTU %d", param->mtu.mtu);
        break;

    case ESP_GATTS_UNREG_EVT:
        break;

    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_TAG, "Service create, status %d, service_handle %d", param->create.status, param->create.service_handle);
        gls_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
        gls_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_128; 
        memcpy((void*)gls_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid128, (void*)MidiCharacteristicUUIDByteReversed, ESP_UUID_LEN_128);

        esp_ble_gatts_start_service(gls_profile_tab[PROFILE_A_APP_ID].service_handle);
        a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE_NR | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(gls_profile_tab[PROFILE_A_APP_ID].service_handle, &gls_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        a_property,
                                                        &gatts_demo_char1_val, NULL);
        if (add_char_ret)
        {
            ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
        }
        break;

    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;

    case ESP_GATTS_ADD_CHAR_EVT: 
    {
        uint16_t length = 0;
        const uint8_t *prf_char;

        ESP_LOGI(GATTS_TAG, "Characteristic add, status %d, attr_handle %d, service_handle %d",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        gls_profile_tab[PROFILE_A_APP_ID].char_handle = param->add_char.attr_handle;
        gls_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gls_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        
        esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle,  &length, &prf_char);
        if (get_attr_ret == ESP_FAIL)
        {
            ESP_LOGE(GATTS_TAG, "ILLEGAL HANDLE");
        }

        ESP_LOGI(GATTS_TAG, "the gatts demo char length = %x", length);
        for(int i = 0; i < length; i++)
        {
            ESP_LOGI(GATTS_TAG, "prf_char[%x] =%x",i,prf_char[i]);
        }
        esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gls_profile_tab[PROFILE_A_APP_ID].service_handle, &gls_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        if (add_descr_ret)
        {
            ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
        }
        break;
    }
    
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        gls_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(GATTS_TAG, "Descriptor add, status %d, attr_handle %d, service_handle %d",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;

    case ESP_GATTS_DELETE_EVT:
        break;

    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "Service start, status %d, service_handle %d",
                 param->start.status, param->start.service_handle);
        break;

    case ESP_GATTS_STOP_EVT:
        break;

    case ESP_GATTS_CONNECT_EVT: 
    {
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));

        // For the IOS system, please reference the apple official documents about the ble connection parameters restrictions
        conn_params.latency = 0;
        conn_params.max_int = 8;       // max_int (x 1.25ms)
        conn_params.min_int = 6;       // min_int (x 1.25ms)
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms

        // start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);

        ESP_LOGI(GATTS_TAG, "Connected, conn_id %u, remote "ESP_BD_ADDR_STR"", param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        
        // start security connect with peer device when receive the connect event sent by the master
        esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);

        break;
    }

    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(GATTS_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x", ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        control_set_bt_status(0);
        break;

    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_TAG, "Confirm receive, status %d, attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK)
        {
            ESP_LOG_BUFFER_HEX(GATTS_TAG, param->conf.value, param->conf.len);
        }
        break;

    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) 
    {
        if (param->reg.status == ESP_GATT_OK) 
        {
            gls_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } 
        else 
        {
            ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) 
        {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                gatts_if == gls_profile_tab[idx].gatts_if) 
            {
                if (gls_profile_tab[idx].gatts_cb) 
                {
                    gls_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}


/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) start_scan(void)
{
    stop_scan_done = false;
    Isconnecting = false;
    esp_ble_gap_start_scanning(BT_SCAN_DURATION);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) gattc_profile_a_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) 
    {
        case ESP_GATTC_REG_EVT:
            ESP_LOGI(GATTC_TAG, "REG_EVT");
            esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
            if (scan_ret)
            {
                ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
            }
            break;
        
        /* one device connect successfully, all profiles callback function will get the ESP_GATTC_CONNECT_EVT,
        so must compare the mac address to check which device is connected, so it is a good choice to use ESP_GATTC_OPEN_EVT. */
        case ESP_GATTC_CONNECT_EVT:
            break;

        case ESP_GATTC_OPEN_EVT:
            if (p_data->open.status != ESP_GATT_OK)
            {
                //open failed, ignore the first device, connect the second device
                ESP_LOGE(GATTC_TAG, "connect device failed, status %d", p_data->open.status);
                conn_device_a = false;
                break;
            }

            memcpy(gl_profile_tab.remote_bda, p_data->open.remote_bda, 6);
            gl_profile_tab.conn_id = p_data->open.conn_id;

            ESP_LOGI(GATTC_TAG, "ESP_GATTC_OPEN_EVT conn_id %d, if %d, status %d, mtu %d", p_data->open.conn_id, gattc_if, p_data->open.status, p_data->open.mtu);
            ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
            ESP_LOG_BUFFER_HEX(GATTC_TAG, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
            
            esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req(gattc_if, p_data->open.conn_id);
            if (mtu_ret)
            {
                ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
            }
            break;

        case ESP_GATTC_CFG_MTU_EVT:
            if (param->cfg_mtu.status != ESP_GATT_OK)
            {
                ESP_LOGE(GATTC_TAG,"Config mtu failed");
            }
            
            //ESP_LOGI(GATTC_TAG, "Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
            search_start_handle = 0xFFFF;
            search_end_handle = 0;

            if (esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, NULL) != ESP_OK)
            {
                ESP_LOGE(GATTC_TAG, "Failed to start search for UUID");
            }
            else
            {
                ESP_LOGI(GATTC_TAG, "Searching for Midi Service UUID match");
            }
            break;

        case ESP_GATTC_SEARCH_RES_EVT: 
        {
            //ESP_LOGI(GATTC_TAG, "SEARCH RES: conn_id = %x is primary service %d", p_data->search_res.conn_id, p_data->search_res.is_primary);
            //ESP_LOGI(GATTC_TAG, "start handle %d end handle %d current handle value %d", p_data->search_res.start_handle, p_data->search_res.end_handle, p_data->search_res.srvc_id.inst_id);
            
            if (p_data->search_res.start_handle < search_start_handle)
            {
                search_start_handle = p_data->search_res.start_handle;
            }

            if (p_data->search_res.end_handle > search_end_handle)
            {
                search_end_handle = p_data->search_res.end_handle;
            }

            get_service_a = true;
            gl_profile_tab.service_start_handle = search_start_handle;
            gl_profile_tab.service_end_handle = search_end_handle;
            break;
        }
        
        case ESP_GATTC_SEARCH_CMPL_EVT:
            uint16_t count = 1;
            esp_gatt_status_t res;
            ESP_LOGI(GATTC_TAG, "Search complete for Services");

            if (p_data->search_cmpl.status != ESP_GATT_OK)
            {
                ESP_LOGE(GATTC_TAG, "Search service failed, error status = %x", p_data->search_cmpl.status);
                break;
            }
            
            if (get_service_a)
            {                
                // get descriptors for Midi
                esp_gattc_char_elem_t* char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * 4);
                if (!char_elem_result)
                {
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }
                else 
                {
                    ESP_LOGI(GATTC_TAG, "Finding Characteristics");

                    // init search table
                    uint8_t filter_index = 0;

                    remote_filter_char_uuid_reuse[filter_index].len = ESP_UUID_LEN_128;
                    memcpy((void*)remote_filter_char_uuid_reuse[filter_index].uuid.uuid128, (void*)MidiCharacteristicUUIDByteReversed, ESP_UUID_LEN_128);
                    filter_index++;
                    
                    //remote_filter_char_uuid_reuse[filter_index].len = ESP_UUID_LEN_128;
                    //memcpy((void*)remote_filter_char_uuid_reuse[filter_index].uuid.uuid128, (void*)BatteryLevelCharacteristicUUIDByteReversed, ESP_UUID_LEN_128);
                    //filter_index++;

                    // loop and find all characteristics, and register notification
                    for (int loop = 0; loop < filter_index; loop++)
                    {
                        // find the characteristic
                        count = 1;
                        res = esp_ble_gattc_get_char_by_uuid(gattc_if, 
                                                        p_data->search_cmpl.conn_id, 
                                                        gl_profile_tab.service_start_handle, 
                                                        gl_profile_tab.service_end_handle, 
                                                        remote_filter_char_uuid_reuse[loop], 
                                                        char_elem_result, 
                                                        &count);

                        if (res == ESP_OK)
                        {
                            ESP_LOGI(GATTC_TAG, "Characteristic loop %d get returned %d", loop, count);

                            for (uint32_t character_loop = 0; character_loop < count; character_loop++)
                            {
                                if (char_elem_result[character_loop].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)
                                {
                                    gl_profile_tab.char_handle = char_elem_result[character_loop].char_handle;

                                    if (esp_ble_gattc_register_for_notify(gattc_if,  gl_profile_tab.remote_bda, char_elem_result[character_loop].char_handle) != ESP_OK)
                                    {
                                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_register_for_notify failed %d %d", (int)loop, (int)char_elem_result[character_loop].char_handle);
                                    }
                                    else
                                    {
                                        ESP_LOGI(GATTC_TAG, "esp_ble_gattc_register_for_notify OK %d on handle %d", (int)loop, (int)char_elem_result[character_loop].char_handle);                                

                                        // update UI to show a BT connected
                                        control_set_bt_status(1);
                                    }
                                }  
                            }                   
                        }
                        else
                        {
                            ESP_LOGE(GATTC_TAG, "Failed to find Midi characteristic %d: %d", (int)loop, (int)res);
                        }
                    }

                    free(char_elem_result);
                }
            }
        
            break;

        case ESP_GATTC_REG_FOR_NOTIFY_EVT: 
        {
            if (p_data->reg_for_notify.status != ESP_GATT_OK)
            {
                ESP_LOGE(GATTC_TAG, "reg notify failed, error status =%x", p_data->reg_for_notify.status);
                break;
            }
        
            uint16_t count = 0;
            uint16_t notify_en = 1;

            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                        gl_profile_tab.conn_id,
                                                                        ESP_GATT_DB_DESCRIPTOR,
                                                                        gl_profile_tab.service_start_handle,
                                                                        gl_profile_tab.service_end_handle,
                                                                        //????? to do
                                                                        gl_profile_tab.char_handle,
                                                                        &count);
            if (ret_status != ESP_GATT_OK)
            {
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            
            if (count > 0)
            {
                descr_elem_result_a = (esp_gattc_descr_elem_t *)malloc(sizeof(esp_gattc_descr_elem_t) * count);

                if (!descr_elem_result_a)
                {
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                }
                else
                {
                    ret_status = esp_ble_gattc_get_descr_by_char_handle(gattc_if,
                                                                        gl_profile_tab.conn_id,
                                                                        p_data->reg_for_notify.handle,
                                                                        notify_descr_uuid,
                                                                        descr_elem_result_a,
                                                                        &count);

                    if (ret_status != ESP_GATT_OK)
                    {
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error %d", (int)ret_status);
                    }

                    if (count > 0 && descr_elem_result_a[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result_a[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG)
                    {
                        ret_status = esp_ble_gattc_write_char_descr(gattc_if,
                                                                    gl_profile_tab.conn_id,
                                                                    descr_elem_result_a[0].handle,
                                                                    sizeof(notify_en),
                                                                    (uint8_t*)&notify_en,
                                                                    ESP_GATT_WRITE_TYPE_RSP,
                                                                    ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK)
                    {
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error %d", (int)ret_status);
                    }

                    // free descr_elem_result
                    free(descr_elem_result_a);
                }
            }
            else
            {
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }
            break;
        }

        case ESP_GATTC_NOTIFY_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, Receive notify values. len: %d", (int)p_data->notify.value_len);
            //ESP_LOG_BUFFER_HEX(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);

            // handle Midi data.
            midi_helper_process_incoming_data(p_data->notify.value, p_data->notify.value_len, midi_serial_channel, control_get_config_item_int(CONFIG_ITEM_ENABLE_BT_MIDI_CC));            
            break;

        case ESP_GATTC_WRITE_DESCR_EVT:
            if (p_data->write.status != ESP_GATT_OK)
            {
                ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
                break;
            }
            ESP_LOGI(GATTC_TAG, "write descr success");
            
            uint8_t write_char_data[35];
            for (int i = 0; i < sizeof(write_char_data); ++i)
            {
                write_char_data[i] = i % 256;
            }
            esp_ble_gattc_write_char( gattc_if,
                                    gl_profile_tab.conn_id,
                                    gl_profile_tab.char_handle,
                                    sizeof(write_char_data),
                                    write_char_data,
                                    ESP_GATT_WRITE_TYPE_RSP,
                                    ESP_GATT_AUTH_REQ_NONE);
            break;

        case ESP_GATTC_WRITE_CHAR_EVT:
            if (p_data->write.status != ESP_GATT_OK)
            {
                ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
            }
            else
            {
                ESP_LOGI(GATTC_TAG, "write char success");
            }
            start_scan();
            break;

        case ESP_GATTC_SRVC_CHG_EVT: 
        {
            esp_bd_addr_t bda;
            memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
            ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:%08x%04x",(bda[0] << 24) + (bda[1] << 16) + (bda[2] << 8) + bda[3], (bda[4] << 8) + bda[5]);
            break;
        }
        case ESP_GATTC_DISCONNECT_EVT:
            //Start scanning again
            start_scan();

            // update UI to show a BT disconnected
            control_set_bt_status(0);

            if (memcmp(p_data->disconnect.remote_bda, gl_profile_tab.remote_bda, 6) == 0)
            {
                ESP_LOGI(GATTC_TAG, "Device A disconnected");
                conn_device_a = false;
                get_service_a = false;
            }
            break;
            
        default:
            break;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;

    switch (event) 
    {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~adv_config_flag);
            if (adv_config_done == 0)
            {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;

        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~scan_rsp_config_flag);
            if (adv_config_done == 0)
            {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            //advertising start complete event to indicate advertising start successfully or failed
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) 
            {
                ESP_LOGE(GATTS_TAG, "Advertising start failed, status %d", param->adv_start_cmpl.status);
                break;
            }
            ESP_LOGI(GATTS_TAG, "Advertising start successfully");
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) 
            {
                ESP_LOGE(GATTS_TAG, "Advertising stop failed, status %d", param->adv_stop_cmpl.status);
                break;
            }
            ESP_LOGI(GATTS_TAG, "Advertising stop successfully");
            break;

        case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* passkey request event */
            ESP_LOGI(GATTS_TAG, "Passkey request");
            /* Call the following function to input the passkey which is displayed on the remote device */
            //esp_ble_passkey_reply(heart_rate_profile_tab[HEART_PROFILE_APP_IDX].remote_bda, true, 0x00);
            break;

        case ESP_GAP_BLE_OOB_REQ_EVT: 
        {
            ESP_LOGI(GATTS_TAG, "OOB request");
            uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
            esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
            break;
        }

        case ESP_GAP_BLE_LOCAL_IR_EVT:                               /* BLE local IR event */
            ESP_LOGI(GATTS_TAG, "Local identity root");
            break;

        case ESP_GAP_BLE_LOCAL_ER_EVT:                               /* BLE local ER event */
            ESP_LOGI(GATTS_TAG, "Local encryption root");
            break;

        case ESP_GAP_BLE_NC_REQ_EVT:
            /* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
            show the passkey number to the user to confirm it with the number displayed by peer device. */
            esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
            ESP_LOGI(GATTS_TAG, "Numeric Comparison request, passkey %" PRIu32, param->ble_security.key_notif.passkey);
            break;

        case ESP_GAP_BLE_SEC_REQ_EVT:
            /* send the positive(true) security response to the peer device to accept the security request.
            If not accept the security request, should send the security response with negative(false) accept value*/
            esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
            break;

        case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
            //show the passkey number to the user to input it in the peer device.
            ESP_LOGI(GATTS_TAG, "Passkey notify, passkey %06" PRIu32, param->ble_security.key_notif.passkey);
            break;

        case ESP_GAP_BLE_KEY_EVT:
            //shows the ble key info share with peer device to the user.
            ESP_LOGI(GATTS_TAG, "Key exchanged, key_type %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
            break;

        case ESP_GAP_BLE_AUTH_CMPL_EVT: 
        {
            esp_bd_addr_t bd_addr;
            memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
            ESP_LOGI(GATTS_TAG, "Authentication complete, addr_type %u, addr "ESP_BD_ADDR_STR"", param->ble_security.auth_cmpl.addr_type, ESP_BD_ADDR_HEX(bd_addr));
            
            if (!param->ble_security.auth_cmpl.success) 
            {
                ESP_LOGI(GATTS_TAG, "Pairing failed, reason 0x%x",param->ble_security.auth_cmpl.fail_reason);
            } 
            else 
            {
                ESP_LOGI(GATTS_TAG, "Pairing success, auth_mode %s",esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
            }
            
            show_bonded_devices();

            control_set_bt_status(1);
            break;
        }

        case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: 
        {
            ESP_LOGD(GATTS_TAG, "Bond device remove, status %d, device "ESP_BD_ADDR_STR"", param->remove_bond_dev_cmpl.status, ESP_BD_ADDR_HEX(param->remove_bond_dev_cmpl.bd_addr));
            control_set_bt_status(0);
            break;
        }

        case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
            if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS)
            {
                ESP_LOGE(GATTS_TAG, "Local privacy config failed, status %x", param->local_privacy_cmpl.status);
                break;
            }
            ESP_LOGI(GATTS_TAG, "Local privacy config successfully");
            break;

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(GATTS_TAG, "Connection params update, status %d, conn_int %d, latency %d, timeout %d",
                    param->update_conn_params.status,
                    param->update_conn_params.conn_int,
                    param->update_conn_params.latency,
                    param->update_conn_params.timeout);
            break;

        case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
            ESP_LOGI(GATTS_TAG, "Packet length update, status %d, rx %d, tx %d",
                    param->pkt_data_length_cmpl.status,
                    param->pkt_data_length_cmpl.params.rx_len,
                    param->pkt_data_length_cmpl.params.tx_len);
            break;


        // Client stuff
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: 
        {
            //the unit of the duration is second
            esp_ble_gap_start_scanning(BT_SCAN_DURATION);
            break;
        }
        
        case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
            //scan start complete event to indicate scan start successfully or failed
            if (param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS) 
            {
                ESP_LOGI(GATTC_TAG, "Scan start success");
            }
            else
            {
                ESP_LOGE(GATTC_TAG, "Scan start failed");
            }
            break;

        case ESP_GAP_BLE_SCAN_RESULT_EVT: 
        {
            esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
            
            switch (scan_result->scan_rst.search_evt) 
            {
            case ESP_GAP_SEARCH_INQ_RES_EVT:
                //ESP_LOG_BUFFER_HEX(GATTC_TAG, scan_result->scan_rst.bda, 6);
                //ESP_LOGI(GATTC_TAG, "Searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
                adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
                //ESP_LOGI(GATTC_TAG, "Searched Device Name Len %d", adv_name_len);
                //esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
                //ESP_LOGI(GATTC_TAG, "\n");
                
                if (Isconnecting)
                {
                    break;
                }
                
                if (conn_device_a && !stop_scan_done)
                {
                    stop_scan_done = true;
                    esp_ble_gap_stop_scanning();
                    ESP_LOGI(GATTC_TAG, "Device is connected, stopping scan");
                    break;
                }
                
                if (adv_name != NULL) 
                {
                    if (remote_device_names_length > 0)
                    {
                        // loop over list of enabled devices to connect to
                        for (uint8_t loop = 0; loop < remote_device_names_length; loop++)
                        {       
                            //ESP_LOGI(GATTC_TAG, "Checking for device %s. len: %d %d", remote_device_names[loop], strlen(remote_device_names[loop]), adv_name_len);
                            if (strncmp((char*)adv_name, remote_device_names[loop], adv_name_len) == 0) 
                            {
                                if (conn_device_a == false) 
                                {
                                    conn_device_a = true;
                                    ESP_LOGI(GATTC_TAG, "Searched device %s", remote_device_names[loop]);
                                    esp_ble_gap_stop_scanning();
                                    esp_ble_gattc_open(gl_profile_tab.gattc_if, scan_result->scan_rst.bda, scan_result->scan_rst.ble_addr_type, true);
                                    Isconnecting = true;
                                }
                                break;
                            }
                        }
                    }
                }
                break;
            
            case ESP_GAP_SEARCH_INQ_CMPL_EVT:
                break;

            default:
                break;
            }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(GATTC_TAG, "Scan stop failed");
            break;
        }
        ESP_LOGI(GATTC_TAG, "Stop scan successfully");

        break;

    default:
        break;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    //ESP_LOGI(GATTC_TAG, "EVT %d, gattc if %d, app_id %d", event, gattc_if, param->reg.app_id);

    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) 
    {
        if (param->reg.status == ESP_GATT_OK) 
        {
            gl_profile_tab.gattc_if = gattc_if;
        } 
        else 
        {
            ESP_LOGI(GATTC_TAG, "Reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do 
    {
        if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                gattc_if == gl_profile_tab.gattc_if) 
        {
            if (gl_profile_tab.gattc_cb) 
            {
                gl_profile_tab.gattc_cb(event, gattc_if, param);
            }
        }
    } while (0);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void InitDeviceList(void)
{
    memset((void*)remote_device_names, 0, sizeof(remote_device_names));
    remote_device_names_length = 0;

    // build list of devices to scan for and connect to if found
    if (control_get_config_item_int(CONFIG_ITEM_MV_CHOC_ENABLE))
    {
        // M-vave Chocolate device name is 'FootCtrl'
        strncpy(remote_device_names[remote_device_names_length], "FootCtrl", MAX_DEVICE_NAME_LENGTH);
        remote_device_names_length++;

        // M-vave Chocolate Plus device name is 'FootCtrlPlus'
        strncpy(remote_device_names[remote_device_names_length], "FootCtrlPlus", MAX_DEVICE_NAME_LENGTH);
        remote_device_names_length++;
    }

    if (control_get_config_item_int(CONFIG_ITEM_XV_MD1_ENABLE))
    {
        // Xvive Bluetooth Midi adaptor is 'Xvive MD1'
        strncpy(remote_device_names[remote_device_names_length], "Xvive MD1", MAX_DEVICE_NAME_LENGTH);
        remote_device_names_length++;
    }

    if (control_get_config_item_int(CONFIG_ITEM_CUSTOM_BT_ENABLE))
    {
        // Custom Bluetooth device name
        control_get_config_item_string(CONFIG_ITEM_BT_CUSTOM_NAME, remote_device_names[remote_device_names_length]);
        remote_device_names_length++;
    }

    ESP_LOGI(GATTC_TAG, "Device List length: %d", remote_device_names_length);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void init_BLE(void)
{
    esp_err_t ret;

    ESP_LOGI(TAG, "Midi BLE init start");

    // get the channel to use
    midi_serial_channel = control_get_config_item_int(CONFIG_ITEM_MIDI_CHANNEL);

    // adjust to zero based indexing
    if (midi_serial_channel > 0)
    {
        midi_serial_channel--;
    }

        
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) 
    {
        ESP_LOGE(GATTC_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) 
    {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) 
    {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) 
    {
        ESP_LOGE(GATTC_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    // register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret)
    {
        ESP_LOGE(GATTC_TAG, "gap register error, error code = %x", ret);
        return;
    }

    if (control_get_config_item_int(CONFIG_ITEM_BT_MODE) == BT_MODE_CENTRAL)
    {
        // Client stuff
        ESP_LOGI(GATTC_TAG, "Enabling BT Client mode");

        InitDeviceList();

        // register the callback function to the gattc module
        ret = esp_ble_gattc_register_callback(esp_gattc_cb);
        if(ret)
        {
            ESP_LOGE(GATTC_TAG, "gattc register error, error code = %x", ret);
            return;
        }

        ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
        if (ret)
        {
            ESP_LOGE(GATTC_TAG, "gattc app register error, error code = %x", ret);
            return;
        }
    }
    else if (control_get_config_item_int(CONFIG_ITEM_BT_MODE) == BT_MODE_PERIPHERAL)
    {
        // Server stuff
        ESP_LOGI(GATTS_TAG, "Enabling BT Server mode");

        ret = esp_ble_gatts_register_callback(gatts_event_handler);
        if (ret)
        {
            ESP_LOGE(GATTS_TAG, "gatts register error, error code = %x", ret);
            return;
        }

        ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
        if (ret)
        {
            ESP_LOGE(GATTS_TAG, "gatts app register error, error code = %x", ret);
            return;
        }

        // set the security iocap & auth_req & key size & init key response key parameters to the stack
        esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;     //bonding with peer device after authentication
        esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input

        uint8_t key_size = 16;      //the key size should be 7~16 bytes
        uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
        uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
        
        //set static passkey
        uint32_t passkey = 123456;
        uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
        uint8_t oob_support = ESP_BLE_OOB_DISABLE;

        esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
        
        /* If your BLE device acts as a Slave, the init_key means you hope which types of key of the master should distribute to you,
        and the response key means which key you can distribute to the master;
        If your BLE device acts as a master, the response key means you hope which types of key of the slave should distribute to you,
        and the init key means which key you can distribute to the slave. */
        esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
        esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
    }

    // set MTU
    ret = esp_ble_gatt_set_local_mtu(200);
    if (ret)
    {
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", ret);
    }

    // dump ram usage
    //heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
    //heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void midi_delete_bluetooth_bonds(void)
{
    remove_all_bonded_devices();
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void midi_init(void)
{
    init_BLE();
}
