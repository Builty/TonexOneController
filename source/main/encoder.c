#include "encoder.h"
#include "control.h"
#include <stdint.h>
#include <stdbool.h>

#include "SX1509.h"
#include "tonex_params.h"

#include "esp_log.h"
#include "display.h"
#include "screens.h"

#include <stdio.h>

#define ENC1_A 3
#define ENC1_B 4

#define ENC2_A 5
#define ENC2_B 6

#define ENC1_SW  7
#define ENC2_SW  8

#define MASTER_MIN -40.0f
#define MASTER_MAX   3.0f
#define MASTER_STEP  0.5f

void usb_modify_parameter(uint16_t index, float value);

typedef struct
{
    uint8_t lastState;

    int8_t accumulator;

    float value;

} encoder_t;

static encoder_t enc1;
static encoder_t enc2;

static const int8_t table[16] =
{
     0,-1, 1, 0,
     1, 0, 0,-1,
    -1, 0, 0, 1,
     0, 1,-1, 0
};

static uint8_t enc2ButtonLast = 1;

static TickType_t enc2PressTick = 0;
static void toggleCurrentEffect(void);
static uint8_t readEncoder(uint8_t pinA,uint8_t pinB)
{
    uint8_t a,b;

    SX1509_digitalRead(pinA,&a);
    SX1509_digitalRead(pinB,&b);

    return (a<<1)|b;
}

static void encoder1CW(void)
{
    enc1.value+=MASTER_STEP;

    if(enc1.value>MASTER_MAX)
        enc1.value=MASTER_MAX;

    usb_modify_parameter(
        TONEX_GLOBAL_MASTER_VOLUME,
        enc1.value
    );

    ESP_LOGI("ENCODER","Master %.1f",enc1.value);

            char msg[32];

        snprintf(msg, sizeof(msg),
                "MASTER %.1f dB",
                enc1.value);

        UI_ShowToast(msg);
        }

static void encoder1CCW(void)
{
    enc1.value-=MASTER_STEP;

    if(enc1.value<MASTER_MIN)
        enc1.value=MASTER_MIN;

    usb_modify_parameter(
        TONEX_GLOBAL_MASTER_VOLUME,
        enc1.value
    );

    ESP_LOGI("ENCODER","Master %.1f",enc1.value);

        char msg[32];

        snprintf(msg, sizeof(msg),
                "MASTER %.1f dB",
                enc1.value);

        UI_ShowToast(msg);
        }


static void encoder2CW(void)
    {
        display_adjust_current_parameter(1);
    }

    static void encoder2CCW(void)
    {
        display_adjust_current_parameter(-1);
    }

static void processEncoder1(void)
{
    uint8_t current = readEncoder(ENC1_A, ENC1_B);

    uint8_t index =
        (enc1.lastState << 2) |
        current;

    enc1.accumulator += table[index];

    if(enc1.accumulator >= 4)
    {
        encoder1CW();
        enc1.accumulator = 0;
    }

    if(enc1.accumulator <= -4)
    {
        encoder1CCW();
        enc1.accumulator = 0;
    }

    enc1.lastState = current;
}

static void processEncoder2(void)
{
    uint8_t current = readEncoder(ENC2_A, ENC2_B);

    uint8_t index =
        (enc2.lastState << 2) |
        current;

    enc2.accumulator += table[index];

    if(enc2.accumulator >= 4)
    {
        encoder2CW();
        enc2.accumulator = 0;
    }

    if(enc2.accumulator <= -4)
    {
        encoder2CCW();
        enc2.accumulator = 0;
    }

    enc2.lastState = current;
}

static void encoderTask(void *arg)
{
        while(true)
    {
        processEncoder1();
        processEncoder2();

        SX1509_refresh();

        uint8_t sw;
        SX1509_digitalRead(ENC2_SW, &sw);

        if(enc2ButtonLast && !sw)
        {
            enc2PressTick = xTaskGetTickCount();
        }

        if(!enc2ButtonLast && sw)
        {
            TickType_t pressTime = xTaskGetTickCount() - enc2PressTick;

            if(pressTime > pdMS_TO_TICKS(700))
            {
                toggleCurrentEffect();
            }
            else
            {
                ESP_LOGI("ENCODER", "SHORT PRESS");           
            }
        }

        enc2ButtonLast = sw;

        vTaskDelay(pdMS_TO_TICKS(2));
    }
}
void encoder_init(void)
{
    /*
        Encoder1
    */
    SX1509_gpioMode(ENC1_A, EXPANDER_INPUT_PULLUP);
    SX1509_gpioMode(ENC1_B, EXPANDER_INPUT_PULLUP);

    /*
        Encoder2
    */
    SX1509_gpioMode(ENC2_A, EXPANDER_INPUT_PULLUP);
    SX1509_gpioMode(ENC2_B, EXPANDER_INPUT_PULLUP);


    /*
        Encoder Switches
    */
    SX1509_gpioMode(ENC1_SW, EXPANDER_INPUT_PULLUP);
    SX1509_gpioMode(ENC2_SW, EXPANDER_INPUT_PULLUP);

    SX1509_refresh();

    enc1.lastState = readEncoder(ENC1_A, ENC1_B);
    enc2.lastState = readEncoder(ENC2_A, ENC2_B);

    enc1.accumulator = 0;
    enc2.accumulator = 0;
    
    /*
        현재 마스터 볼륨 값으로 시작
    */
    tModellerParameter *param_ptr;

    if(control_get_connected_modeller_params_locked_access(&param_ptr) == ESP_OK)
    {
        enc1.value = param_ptr[TONEX_GLOBAL_MASTER_VOLUME].Value;

        control_release_connected_modeller_params_locked_access();
    }
    else
    {
        enc1.value = -10.0f;
    }

    xTaskCreatePinnedToCore(
        encoderTask,
        "encoder",
        4096,
        NULL,
        5,
        NULL,
        1);

    ESP_LOGI("ENCODER", "Encoder task started");
}




static void toggleCurrentEffect(void)
{
    tModellerParameter *param_ptr;

    if (control_get_connected_modeller_params_locked_access(&param_ptr) != ESP_OK)
        return;

    uint16_t param = 0;

    switch (display_get_current_config_tab())
    {
        case CONFIG_TAB_GATE:
            param = TONEX_PARAM_NOISE_GATE_ENABLE;
            break;

        case CONFIG_TAB_COMPRESSOR:
            param = TONEX_PARAM_COMP_ENABLE;
            break;

        case CONFIG_TAB_MODULATION:
            param = TONEX_PARAM_MODULATION_ENABLE;
            break;

        case CONFIG_TAB_DELAY:
            param = TONEX_PARAM_DELAY_ENABLE;
            break;

        case CONFIG_TAB_REVERB:
            param = TONEX_PARAM_REVERB_ENABLE;
            break;

        case CONFIG_TAB_AMPLIFIER:
            param = TONEX_PARAM_MODEL_AMP_ENABLE;
            break;

        default:
            control_release_connected_modeller_params_locked_access();
            return;
    }

    float newValue = (param_ptr[param].Value != 0.0f) ? 0.0f : 1.0f;

    control_release_connected_modeller_params_locked_access();

    usb_modify_parameter(param, newValue);
}





