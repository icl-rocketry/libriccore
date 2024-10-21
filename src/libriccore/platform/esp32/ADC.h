#pragma once
/**
 * @file ADC.h
 * @author Soham More
 * @brief ADC wrapper for ESP32
 * @date 2024
 */

#include <stdint.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <esp32-hal-gpio.h>

// ADC reference voltage = 1100mV, however, this can range between 1000mV and 1200mV.

class ADC
{

public:
    ADC(const uint8_t pin) : _pin(pin)
                                {};

    void setup()
    {

        // Configure ADC channel

        int8_t channel = digitalPinToAnalogChannel(_pin);
        _channel = static_cast<adc1_channel_t>(channel);

        adc1_config_width(width);
        adc1_config_channel_atten(_channel, m_atten);

        esp_adc_cal_characterize(_unit, m_atten, width, VREF, &_adcCal);
        _adcInitialized = true;
    }

    void update()
    {

        if (!_adcInitialized)
        {
            return;
        }

        adc1_raw = adc1_get_raw(_channel);
    }

    int16_t getADC()
    {
        return adc1_raw;
    }

    void setAttenuation(adc_atten_t attenuation){m_atten = attenuation;};
    const uint8_t _pin;
    adc_channel_t channel;
    adc1_channel_t _channel;

private:
    adc_unit_t _unit = ADC_UNIT_1;
    esp_adc_cal_characteristics_t _adcCal;
    static constexpr adc_bits_width_t width = ADC_WIDTH_BIT_12;
    adc_atten_t m_atten = ADC_ATTEN_DB_11;
    bool _adcInitialized = false;
    static constexpr int VREF = 1100;
    uint16_t adc1_raw;
    uint16_t adc2_raw;
};