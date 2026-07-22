#include "encoder.h"

#include <stdint.h>
#include <stdbool.h>

#include "SX1509.h"
#include "tonex_params.h"

#include "esp_log.h"

#include "screens.h"
#define ENC1_A 3
#define ENC1_B 4

#define ENC2_A 5
#define ENC2_B 6

#define MASTER_MIN -40.0f
#define MASTER_MAX   3.0f
#define MASTER_STEP  0.5f

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
}


static void encoder2CW(void)
{
    ESP_LOGI("ENCODER","Encoder2 CW");
}

static void encoder2CCW(void)
{
    ESP_LOGI("ENCODER","Encoder2 CCW");
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

    SX1509_refresh();

    enc1.lastState = readEncoder(ENC1_A, ENC1_B);
    enc2.lastState = readEncoder(ENC2_A, ENC2_B);

    enc1.accumulator = 0;
    enc2.accumulator = 0;
    
    /*
        현재 마스터 볼륨 값으로 시작
    */
    enc1.value = -10.0f;

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

void usb_modify_parameter(uint16_t index, float value);