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


#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define APP_VERSION		"2.0.0.2"

#define I2C_MASTER_NUM_1                0          
#define I2C_MASTER_NUM_2                1          

extern SemaphoreHandle_t I2CMutex_1;
extern SemaphoreHandle_t I2CMutex_2;

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_9       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_8       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1

    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // IO expander
    #define FOOTSWITCH_1		IO_EXPANDER_PIN_1
    #define TOUCH_RESET 		IO_EXPANDER_PIN_2
    #define LCD_BACKLIGHT		IO_EXPANDER_PIN_3
    #define LCD_RESET    		IO_EXPANDER_PIN_4
    #define SD_CS       		IO_EXPANDER_PIN_5
    #define FOOTSWITCH_2		IO_EXPANDER_PIN_6

    // these 2 unsupported
    #define FOOTSWITCH_3		-1
    #define FOOTSWITCH_4		-1

    // Micro pins
    #define TOUCH_INT           GPIO_NUM_4    // touch panel interrupt

    // Midi: RS485 port
    #define UART_RX_PIN         GPIO_NUM_43
    #define UART_TX_PIN         GPIO_NUM_44 

    // Midi on Waveshare 7" using RS485 port (dev only)
    //#define UART_RX_PIN       GPIO_NUM_15 
    //#define UART_TX_PIN       GPIO_NUM_16 

    // Midi: ADC port on 4.3 dev board (dev only)
    //#define UART_RX_PIN         GPIO_NUM_6
    //#define UART_TX_PIN         -1

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define DISPLAY_PIN_NUM_BK_LIGHT       -1
    #define DISPLAY_PIN_NUM_HSYNC          46
    #define DISPLAY_PIN_NUM_VSYNC          3
    #define DISPLAY_PIN_NUM_DE             5
    #define DISPLAY_PIN_NUM_PCLK           7
    #define DISPLAY_PIN_NUM_DATA0          14 // B3
    #define DISPLAY_PIN_NUM_DATA1          38 // B4
    #define DISPLAY_PIN_NUM_DATA2          18 // B5
    #define DISPLAY_PIN_NUM_DATA3          17 // B6
    #define DISPLAY_PIN_NUM_DATA4          10 // B7
    #define DISPLAY_PIN_NUM_DATA5          39 // G2
    #define DISPLAY_PIN_NUM_DATA6          0 // G3
    #define DISPLAY_PIN_NUM_DATA7          45 // G4
    #define DISPLAY_PIN_NUM_DATA8          48 // G5
    #define DISPLAY_PIN_NUM_DATA9          47 // G6
    #define DISPLAY_PIN_NUM_DATA10         21 // G7
    #define DISPLAY_PIN_NUM_DATA11         1  // R3
    #define DISPLAY_PIN_NUM_DATA12         2  // R4
    #define DISPLAY_PIN_NUM_DATA13         42 // R5
    #define DISPLAY_PIN_NUM_DATA14         41 // R6
    #define DISPLAY_PIN_NUM_DATA15         40 // R7
    #define DISPLAY_PIN_NUM_DISP_EN        -1

    // Pin assignments for SD Card
    #define PIN_NUM_MISO        13
    #define PIN_NUM_MOSI        11
    #define PIN_NUM_CLK         12
    #define PIN_NUM_CS          -1

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY
    // Waveshare 4.3 (not B) dual USB port board with resistor mod.
    // Development use only, not for release.
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_9       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_8       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1
    
    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // IO expander
    #define FOOTSWITCH_1		-1
    #define TOUCH_RESET 		IO_EXPANDER_PIN_1
    #define LCD_BACKLIGHT		IO_EXPANDER_PIN_2
    #define LCD_RESET    		IO_EXPANDER_PIN_3
    #define SD_CS       		IO_EXPANDER_PIN_4
    #define FOOTSWITCH_2		-1

    // these 2 unsupported
    #define FOOTSWITCH_3		-1
    #define FOOTSWITCH_4		-1

    // Micro pins
    #define TOUCH_INT           GPIO_NUM_4    // touch panel interrupt

    // Midi: ADC port
    #define UART_RX_PIN         GPIO_NUM_6
    #define UART_TX_PIN         -1

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define DISPLAY_PIN_NUM_BK_LIGHT       -1
    #define DISPLAY_PIN_NUM_HSYNC          46
    #define DISPLAY_PIN_NUM_VSYNC          3
    #define DISPLAY_PIN_NUM_DE             5
    #define DISPLAY_PIN_NUM_PCLK           7
    #define DISPLAY_PIN_NUM_DATA0          14 // B3
    #define DISPLAY_PIN_NUM_DATA1          38 // B4
    #define DISPLAY_PIN_NUM_DATA2          18 // B5
    #define DISPLAY_PIN_NUM_DATA3          17 // B6
    #define DISPLAY_PIN_NUM_DATA4          10 // B7
    #define DISPLAY_PIN_NUM_DATA5          39 // G2
    #define DISPLAY_PIN_NUM_DATA6          0 // G3
    #define DISPLAY_PIN_NUM_DATA7          45 // G4
    #define DISPLAY_PIN_NUM_DATA8          48 // G5
    #define DISPLAY_PIN_NUM_DATA9          47 // G6
    #define DISPLAY_PIN_NUM_DATA10         21 // G7
    #define DISPLAY_PIN_NUM_DATA11         1  // R3
    #define DISPLAY_PIN_NUM_DATA12         2  // R4
    #define DISPLAY_PIN_NUM_DATA13         42 // R5
    #define DISPLAY_PIN_NUM_DATA14         41 // R6
    #define DISPLAY_PIN_NUM_DATA15         40 // R7
    #define DISPLAY_PIN_NUM_DISP_EN        -1

    // Pin assignments for SD Card
    #define PIN_NUM_MISO        13
    #define PIN_NUM_MOSI        11
    #define PIN_NUM_CLK         12
    #define PIN_NUM_CS          -1

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_10       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_11       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1

    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_3
    #define FOOTSWITCH_2		GPIO_NUM_2
    #define FOOTSWITCH_3		GPIO_NUM_44     // same as UART RX
    #define FOOTSWITCH_4		GPIO_NUM_44     // not enough IO, dummy value

    // Midi
    #define UART_RX_PIN         GPIO_NUM_18 
    #define UART_TX_PIN         GPIO_NUM_17 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define WAVESHARE_240_280_LCD_GPIO_SCLK           (GPIO_NUM_6)
    #define WAVESHARE_240_280_LCD_GPIO_MOSI           (GPIO_NUM_7)
    #define WAVESHARE_240_280_LCD_GPIO_RST            (GPIO_NUM_8)
    #define WAVESHARE_240_280_LCD_GPIO_DC             (GPIO_NUM_4)
    #define WAVESHARE_240_280_LCD_GPIO_CS             (GPIO_NUM_5)
    #define WAVESHARE_240_280_LCD_GPIO_BL             (GPIO_NUM_15)

    #define WAVESHARE_240_280_BUZZER                  GPIO_NUM_42

    // Touch
    #define TOUCH_RESET 		GPIO_NUM_13
    #define TOUCH_INT           GPIO_NUM_14

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_10       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_11       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1

    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_16
    #define FOOTSWITCH_2		GPIO_NUM_3
    #define FOOTSWITCH_3		GPIO_NUM_2
    #define FOOTSWITCH_4		GPIO_NUM_44     // same as UART RX

    // Midi
    #define UART_RX_PIN         GPIO_NUM_18 
    #define UART_TX_PIN         GPIO_NUM_17 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define WAVESHARE_240_280_LCD_GPIO_SCLK           (GPIO_NUM_6)
    #define WAVESHARE_240_280_LCD_GPIO_MOSI           (GPIO_NUM_7)
    #define WAVESHARE_240_280_LCD_GPIO_RST            (GPIO_NUM_8)
    #define WAVESHARE_240_280_LCD_GPIO_DC             (GPIO_NUM_4)
    #define WAVESHARE_240_280_LCD_GPIO_CS             (GPIO_NUM_5)
    #define WAVESHARE_240_280_LCD_GPIO_BL             (GPIO_NUM_15)

    // Buzzer is GPIO42 on PCB V2. GPIO33 on PCV V1.
    // note here: GPIO33 used on V1 PCB conflicts with Octal mode PSRAM, so can't use that
    #define WAVESHARE_240_280_BUZZER                  GPIO_NUM_42

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_ZERO
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_10       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_11       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1
    
    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_4
    #define FOOTSWITCH_2		GPIO_NUM_6
    #define FOOTSWITCH_3		GPIO_NUM_2
    #define FOOTSWITCH_4		GPIO_NUM_1 

    // Midi
    #define UART_RX_PIN         GPIO_NUM_5
    #define UART_TX_PIN         GPIO_NUM_7

    // leds
    #define LED_OUTPUT_GPIO_NUM    GPIO_NUM_21

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_DEVKITC
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_10       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_11       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1
    
    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_4
    #define FOOTSWITCH_2		GPIO_NUM_6
    #define FOOTSWITCH_3		GPIO_NUM_2
    #define FOOTSWITCH_4		GPIO_NUM_1 

    // Midi
    #define UART_RX_PIN         GPIO_NUM_5
    #define UART_TX_PIN         GPIO_NUM_7

    // leds
    #define LED_OUTPUT_GPIO_NUM   GPIO_NUM_48

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R    
    // I2C bus 1 
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_0       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_45       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  GPIO_NUM_1       
    #define I2C_MASTER_2_SDA_IO  GPIO_NUM_2       
    
    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_2
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_2 

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_5
    #define FOOTSWITCH_2		GPIO_NUM_6
    #define FOOTSWITCH_3		GPIO_NUM_7
    #define FOOTSWITCH_4		GPIO_NUM_8

    // Midi
    #define UART_RX_PIN         GPIO_NUM_38 
    #define UART_TX_PIN         GPIO_NUM_39 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define ATOM3SR_LCD_GPIO_SCLK           (GPIO_NUM_15)
    #define ATOM3SR_LCD_GPIO_MOSI           (GPIO_NUM_21)
    #define ATOM3SR_LCD_GPIO_RST            (GPIO_NUM_48)
    #define ATOM3SR_LCD_GPIO_DC             (GPIO_NUM_42)
    #define ATOM3SR_LCD_GPIO_CS             (GPIO_NUM_14)
    // Note LCD Backlight uses led driver on I2C
    
#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_17       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_18       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1

    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_1
    #define FOOTSWITCH_2		GPIO_NUM_2
    #define FOOTSWITCH_3		GPIO_NUM_3
    #define FOOTSWITCH_4		GPIO_NUM_10

    // Midi
    #define UART_RX_PIN         GPIO_NUM_11 
    #define UART_TX_PIN         GPIO_NUM_12 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // battery power pin
    #define LG_TDISP_S3_POWERON        GPIO_NUM_15

    // LCD pins
    #define LG_TDISP_S3_TFT_BL         GPIO_NUM_38
    #define LG_TDISP_S3_TFT_DATA0      GPIO_NUM_39
    #define LG_TDISP_S3_TFT_DATA1      GPIO_NUM_40
    #define LG_TDISP_S3_TFT_DATA2      GPIO_NUM_41
    #define LG_TDISP_S3_TFT_DATA3      GPIO_NUM_42
    #define LG_TDISP_S3_TFT_DATA4      GPIO_NUM_45
    #define LG_TDISP_S3_TFT_DATA5      GPIO_NUM_46
    #define LG_TDISP_S3_TFT_DATA6      GPIO_NUM_47
    #define LG_TDISP_S3_TFT_DATA7      GPIO_NUM_48
    #define LG_TDISP_S3_TFT_RST        GPIO_NUM_5
    #define LG_TDISP_S3_TFT_CS         GPIO_NUM_6
    #define LG_TDISP_S3_TFT_DC         GPIO_NUM_7
    #define LG_TDISP_S3_TFT_WR         GPIO_NUM_8
    #define LG_TDISP_S3_TFT_RD         GPIO_NUM_9

    #define LG_TDISP_S3_TOUCH_IRQ      GPIO_NUM_16
    #define LG_TDISP_S3_TOUCH_RST      GPIO_NUM_21

    // Touch
    #define TOUCH_RESET 		       LG_TDISP_S3_TOUCH_RST
    #define TOUCH_INT                  LG_TDISP_S3_TOUCH_IRQ

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_19TOUCH
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_48       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_47       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1

    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_1
    #define FOOTSWITCH_2		GPIO_NUM_2
    #define FOOTSWITCH_3		GPIO_NUM_3
    #define FOOTSWITCH_4		GPIO_NUM_4

    // Midi
    #define UART_RX_PIN         GPIO_NUM_5 
    #define UART_TX_PIN         GPIO_NUM_6 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1
    
    // LCD pins
    #define WAVESHARE_19_LCD_GPIO_SCLK           GPIO_NUM_10
    #define WAVESHARE_19_LCD_GPIO_MOSI           GPIO_NUM_13
    #define WAVESHARE_19_LCD_GPIO_RST            GPIO_NUM_9
    #define WAVESHARE_19_LCD_GPIO_DC             GPIO_NUM_11
    #define WAVESHARE_19_LCD_GPIO_CS             GPIO_NUM_12
    #define WAVESHARE_19_LCD_GPIO_BL             GPIO_NUM_14

    // Touch
    #define TOUCH_RESET 		                 GPIO_NUM_17
    #define TOUCH_INT                            GPIO_NUM_21

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_35B
    // I2C bus 1
    #define I2C_MASTER_1_SCL_IO  GPIO_NUM_7       
    #define I2C_MASTER_1_SDA_IO  GPIO_NUM_8       

    // I2C bus 2
    #define I2C_MASTER_2_SCL_IO  -1
    #define I2C_MASTER_2_SDA_IO  -1

    #define EXTERNAL_IO_EXPANDER_BUS       I2C_MASTER_NUM_1
    #define EXTERNAL_IO_EXPANDER_MUTEX     I2CMutex_1 

    // IO expander
    #define CAM_PWDN       		IO_EXPANDER_PIN_0
    #define LCD_RESET    		IO_EXPANDER_PIN_1
    #define TOUCH_INT    		IO_EXPANDER_PIN_2
    #define SD_CS       		IO_EXPANDER_PIN_3

    // to do
    #define FOOTSWITCH_1		-1  //GPIO_NUM_38
    #define FOOTSWITCH_2		-1  //GPIO_NUM_39
    #define FOOTSWITCH_3		-1  // GPIO_NUM_40
    #define FOOTSWITCH_4		-1  //GPIO_NUM_41

    // Midi: 
    #define UART_RX_PIN         GPIO_NUM_17
    #define UART_TX_PIN         GPIO_NUM_18 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define WAVESHARE_35_LCD_GPIO_SCLK           GPIO_NUM_5
    #define WAVESHARE_35_LCD_GPIO_QSPI_0         GPIO_NUM_1
    #define WAVESHARE_35_LCD_GPIO_QSPI_1         GPIO_NUM_2
    #define WAVESHARE_35_LCD_GPIO_QSPI_2         GPIO_NUM_3
    #define WAVESHARE_35_LCD_GPIO_QSPI_3         GPIO_NUM_4
    #define WAVESHARE_35_LCD_GPIO_RST            GPIO_NUM_NC
    #define WAVESHARE_35_LCD_GPIO_CS             GPIO_NUM_12
    #define WAVESHARE_35_LCD_GPIO_BL             GPIO_NUM_6

    // backlight
    #define WAVESHARE_35_LCD_BL_LEDC_TIMER              LEDC_TIMER_1
    #define WAVESHARE_35_LCD_BL_LEDC_MODE               LEDC_LOW_SPEED_MODE
    #define WAVESHARE_35_LCD_BL_LEDC_CHANNEL            LEDC_CHANNEL_0
    #define WAVESHARE_35_LCD_BL_LEDC_DUTY_RES           LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
    #define WAVESHARE_35_LCD_BL_LEDC_DUTY               (1024)                // Set duty to 50%. (2 ** 13) * 50% = 4096
    #define WAVESHARE_35_LCD_BL_LEDC_FREQUENCY          (10000)          // Frequency in Hertz. Set frequency at 5 kHz


    // Pin assignments for SD Card
    #define PIN_NUM_MISO        GPIO_NUM9
    #define PIN_NUM_MOSI        GPIO_NUM10
    #define PIN_NUM_CLK         GPIO_NUM11

#else
    #error "Unknown hardware platform!"
#endif

esp_err_t i2c_master_reset(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif