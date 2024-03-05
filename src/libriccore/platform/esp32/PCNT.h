#pragma once

#include <std.int>
#include <driver/pcnt.h>
#include <librnp/rnp_networkmanager.h>
#include <string>

class PCNT{

private:
    const std::string _name;
    static constexpr auto LOG_TARGET = RicCoreLoggingConfig::LOGGERS::SYS;
    static const pcnt_unit_t _unit
    int16_t count;
    const uint8_t _gpioSig;
    const uint8_t _channel;

public:

PCNT(std::string_view PCNT_Name, uint8_t channel, uint8_t gpioSig):
_name(PCNT_Name),
_channel(channel),
_gpioSig(gpioSig)
{};

void setup(){

        if (_channel > SOC_PCNT_CHANNELS_PER_UNIT ){
            pcnt_new_unit();
            
        }
    
        /* Prepare configuration for the PCNT unit */
        pcnt_config_t pcnt_config = {
            // Set PCNT input signal and control GPIOs
            .pulse_gpio_num = _gpioSig,
            .ctrl_gpio_num = PCNT_PIN_NOT_USED,
            .pos_mode = PCNT_CHANNEL_EDGE_ACTION_HOLD, // Count up on the positive edge
            .neg_mode = PCNT_CHANNEL_EDGE_ACTION_INCREASE, // Keep the counter value on the negative edge
            .counter_h_lim = 32767,
            .counter_l_lim = 0,
            .unit = _unit,
            .channel = _channel

        };


        /* Initialize PCNT unit */
        pcnt_unit_config(&pcnt_config);
        if(error){
            RicCoreLogging:log<LOG_TARGET>("PCNT Initialisation error");
            return;
        }
        else {
            RicCoreLogging::log<LOG_TARGET>("PCNT Initialised")
        }

        // Resetting count (PCNT bug)
        pcnt_counter_pause(_unit);
        pcnt_counter_clear(_unit);
        pcnt_counter_resume(_unit);
    
}

void update(){

    pcnt_get_counter_value(_unit, &count);
    if (err != ESP_OK){
        RicCoreLogging::log<LOG_TARGET>("PCNT update failed");
        return;
    }

    count = err;

}
esp_err_t getCount(){

    return count;
}

};