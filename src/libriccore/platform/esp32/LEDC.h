#pragma once
#include <stdint.h>
#include <driver/ledc.h>
#include <esp32-hal-gpio.h>
#include <esp_err.h>

    #define LEDC_MODE               LEDC_LOW_SPEED_MODE
    #define LEDC_CHANNEL            LEDC_CHANNEL_0  
    #define LEDC_TIMER              LEDC_TIMER_0  
    #define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
    #define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz
    #define LEDC_OUTPUT_IO          (13) // Define the output GPIO  
    #define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
  

class LEDC
{

    public:

    LEDC(const uint8_t pin) : _pin(pin)
                                {};

void setup()
    {

        // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       =  LEDC_LOW_SPEED_MODE,
        .duty_resolution  =  LEDC_TIMER_13_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = (1000),  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = (13),
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0  ,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    }

    void update()
    {

    }

    private:

    const uint8_t _pin;

};