#pragma once
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <driver/gpio.h>
#include <esp-adc-hal.h>
#include <libriccore/riccorelogging.h>


// ADC reference voltage = 1100mV, however, this can range between 1000mV and 1200mV.
// Calibration values are stored in the eFuse or provided by the user using two point (V1, V2) readings.

class ADC {

private:
    static const adc_unit_t _unit;
    static constexpr adc_atten_t atten = ADC_ATTEN_DB_11;
    static const adc1_channel_t adc1_channel;
    static const adc2_channel_t adc2_channel;
    static constexpr adc_bits_width_t width;
    const uint8_t pin;
    bool _adcInitialized = false;
    

    struct adc_raw{
        int16_t adc1_raw;
        int16_t adc2_raw;
        };

public:
    ADC(ADC_Channel_t ADC_Channel, const uint8_t pin);

void calibration(){
    
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        RicCoreLogging::log<LOGGING_TARGET>("eFuse Two Point: Supported\n");
        
    } else {
        RicCoreLogging::log<LOGGING_TARGET>("eFuse Two Point: NOT supported\n");
    }

   //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        RicCoreLogging::log<LOGGING_TARGET>("eFuse Vref: Supported\n");

    } else {
        RicCoreLogging::log<LOGGING_TARGET>("eFuse Vref: NOT supported\n");
    }

    //Configure ADC

    if (_unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(adc1_channel, atten);
    } else {
        adc2_config_width(width);
        adc2_config_channel_atten((adc2_channel_t)adc2_channel, atten);
    }

    _adcInitialized = true;

}

void update(){

    if (!_adcInitialized) {
        return;
    }
    
    if (_unit = ADC_UNIT_1) {
         adc1_raw = adc1_get_raw(adc1_channel_t channel);
    } 
    else {
    adc2_raw = adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width,&adc2_raw);
    }   
}

int16_t getADC1(){

    return adc1_channel;
}

int16_t getADC2(){

    return adc2_channel;

};