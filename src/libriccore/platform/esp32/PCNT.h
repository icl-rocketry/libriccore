#pragma once

/**
 * @file PCNT.h
 * @author Soham More
 * @brief PCNT wrapper for ESP32
 * @date 2024
 */

#include <stdint.h>
#include <driver/pcnt.h>

class PCNT{

public:

PCNT(pcnt_unit_t _unit, pcnt_channel_t _channel, uint8_t _gpioSig):
_unit(_unit),
_channel(_channel),
_gpioSig(_gpioSig)
{};

void setup(){
        /* Prepare configuration for the PCNT unit */
        pcnt_config_t pcnt_config = {
            // Set PCNT input signal and control GPIOs
            .pulse_gpio_num = _gpioSig,
            .ctrl_gpio_num = PCNT_PIN_NOT_USED,
            .pos_mode = PCNT_CHANNEL_EDGE_ACTION_INCREASE, // Count up on the positive edge
            .neg_mode = PCNT_CHANNEL_EDGE_ACTION_INCREASE, // Keep the counter value on the negative edge
            .counter_h_lim = 32767,
            .counter_l_lim = 0,
            .unit = _unit,
            .channel = _channel

        };


        /* Initialize PCNT unit */
        pcnt_unit_config(&pcnt_config);

        // Resetting count (PCNT bug)
        pcnt_counter_pause(_unit);
        pcnt_counter_clear(_unit);
        pcnt_counter_resume(_unit);
    
}

void update(){

    pcnt_get_counter_value(_unit, &count);

}
esp_err_t getCount(){

    return count;
}

 pcnt_unit_t _unit;
 pcnt_channel_t _channel;
 uint8_t _gpioSig;

private:

    int16_t count;
   
    

};