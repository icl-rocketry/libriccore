#pragma once
/**
 * @file ADC.h
 * @author Soham More
 * @brief ADC wrapper for ESP32
 * @date 2024
 */


#include <string>
#include <stdio.h>
#include <stdint.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <driver/gpio.h>
#include <esp-adc-hal.h>
#include <libriccore/riccorelogging.h>
#include <librnp/rnp_networkmanager.h>


// ADC reference voltage = 1100mV, however, this can range between 1000mV and 1200mV.

class ADC {

private:

    //
    const std::string _name;
    static constexpr auto LOG_TARGET = RicCoreLoggingConfig::LOGGERS::SYS;  
    static const adc_unit_t _unit;
    static constexpr adc_atten_t atten = ADC_ATTEN_DB_11;
    esp_adc_cal_characteristics_t _adcCal;
    static constexpr adc_bits_width_t width;
    static constexpr adc_atten_t _atten = ADC_ATTEN_DB_11;
    const uint8_t _pin;
    bool _adcInitialized = false;
    static constexpr int VREF = 1100;
    uint16_t adc1_raw;
    uint16_t adc2_raw;

public:
    ADC(std::string_view ADC_name const uint8_t pin):
    _name(ADC_name),
    _pin(pin)
    {}; 

void setup(){

    int error = 0;
    int channel = digitalPinToAnalogChannel(_pin);
    //Configure ADC channel
    if (channel > (SOC_ADC_MAX_CHANNEL_NUM - 1)) 
    {
        _channel = static_cast<adc_channel_t>(channel-SOC_ADC_MAX_CHANNEL_NUM);
        _unit = ADC_UNIT_2;

        error += adc2_config_channel_atten(static_cast<adc_channel_t>(_channel), _atten);
    }
    else 
    {
        _channel = static_cast<adc_channel_t>(channel);
        _unit = ADC_UNIT_1;
        error += adc1_config_width(width);
        error += adc1_config_channel_atten(static_cast<adc_channel_t>(_channel), _atten);
    }

    if (error) 
    {
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("ADC setup failed");
        return;
    }

    esp_adc_cal_characterize(_unit, _atten, width, VREF, &_adcCal);
    _adcInitialized = true;
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("ADC setup complete");

}

void update(){

    if (!_adcInitialized) {
        return;
    }
    
    if (_unit == ADC_UNIT_1) {
         adc1_raw = adc1_get_raw(adc1_channel_t channel);
    } 
    else {
    adc2_raw = adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width,&adc2_raw);
    }   
}

int16_t getADC(adc_unit_t _unit, adc_channel_t channel){

    if (_unit == 1) {
        return adc1_raw;
    } 
    else {
        return adc2_raw;
    }
}

};