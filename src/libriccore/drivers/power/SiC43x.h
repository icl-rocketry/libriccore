/**
 * @file SiC43x.h
 * @author Andrei Paduraru
 * @brief Class to manage Vishay SiC43x buck converter devices.
 * @version 0.3
 * @date 2025-02-01
 *
 * @copyright Copyright (c) 2023-2025
 *
 */

#pragma once
#include <stdint.h>

#include <librrc/HAL/arduinogpio.h>
#include "Config/types.h"

//TODO switch this out for an ADC HAL implementation
#include "Sensors/adc_vrailmonitor.h"

class SiC43x
{

public:
    /**
     * @brief Class constructor
     *
     * @param PGood PowerGood pin number
     * @param EN Enable pin number
     * @param defaultEN To determine whether the converter should default on or off
     * @param invertEN To determine whether EN output is inverted, i.e. if writing the pin HIGH would cause the converter to turn off
     * @param VRead Voltage output analog read pin
     * @param HighResistor Resistance value of pontential divider resistor connected to the top side
     * @param LowResistor Resistance value of potential divider resistor connected to the low side
     */

    SiC43x(Types::CoreTypes::SystemStatus_t &systemstatus, int8_t PGood = -1, int8_t EN = -1, bool defaultEN = 0, bool invertEN = 0, int8_t VRead = -1, float HighResistor = 0, float LowResistor = 1) : 
        m_systemstatus(systemstatus),
        m_PGoodPin(PGood),
        m_ENPin(EN),
        m_defaultEN(defaultEN),
        m_invertEN(invertEN),
        m_VReadPin(VRead),
        m_servoVoltage("Servo Voltage", VRead, HighResistor, LowResistor){};

    void setup()
    {
        if (m_PGoodPin >= 0)
        {
            pinMode(m_PGoodPin, INPUT);
        }
        if (m_ENPin >= 0)
        {
            pinMode(m_ENPin, OUTPUT);
            setEN(m_defaultEN);
        }
        if (m_VReadPin >= 0)
        {
            //!TODO - make voltage limits configurable
            m_servoVoltage.setup(12, 5, 4);
        }
    };

    /**
     * @brief Method enables and disables buck converter chip based on level param and invertEN attribute
     *
     * @param Bucklevel Desired status of the buck, true - enabled; false - disabled.
     */
    void setEN(bool Bucklevel)
    {
        bool pinLevel = m_invertEN ^ Bucklevel;
        digitalWrite(m_ENPin, pinLevel);
    };

    /**
     * @brief Method to update output voltage and PGOOD values. Also handles the looping for the restart method to make it non blocking
     */
    void update()
    {
        if (m_VReadPin >= 0)
        {
            m_servoVoltage.update(OutputV);

            //!TODO - make voltage limits configurable
            if ((OutputV > 12 || OutputV < 4) && !m_systemstatus.flagSet(SYSTEM_FLAG::ERROR_VBOUNDS))
            {
                m_systemstatus.newFlag(SYSTEM_FLAG::ERROR_VBOUNDS, "Buck output voltage off-nominal! (ADC reading out of range)");
            }
            else if(OutputV < 12 && OutputV > 4 && m_systemstatus.flagSet(SYSTEM_FLAG::ERROR_VBOUNDS)){
                m_systemstatus.deleteFlag(SYSTEM_FLAG::ERROR_VBOUNDS, "Buck output voltage returned to nominal! (ADC reading back in configured range)");
            }
        }

        if (m_PGoodPin >= 0)
        {
            PGOOD = digitalRead(m_PGoodPin);
            if (!PGOOD && !m_systemstatus.flagSet(SYSTEM_FLAG::ERROR_PGOOD))
            {
                m_systemstatus.newFlag(SYSTEM_FLAG::ERROR_PGOOD, "Buck output voltage off-nominal! (PGOOD deasserted)");
            }
            else if(PGOOD && m_systemstatus.flagSet(SYSTEM_FLAG::ERROR_PGOOD)){
                m_systemstatus.deleteFlag(SYSTEM_FLAG::ERROR_PGOOD, "Buck output voltage returned to nominal! (PGOOD reasserted)");
            }
        }

        if (m_restartFlag == 1)
        {
            restart(m_offTime);
        }
    };

    /**
     * @brief Method to restart the buck converter. Requires update to be called continuously for accurate timing.
     *
     * @param offTime Time the converter should be turned off for in milliseconds.
     */

    void restart(uint32_t offTime)
    {

        if (m_restartFlag == 0)
        {
            m_prevtime = millis(); // time taken at the start of a restart routine, i.e. when the buck is just on normally
            m_offTime = offTime;
        }

        m_restartFlag = 1; // set the flag to 1 so the method gets called again during update

        setEN(false); // turn the buck off

        if (millis() - m_prevtime > offTime)
        {
            setEN(true);       // turn it back on after the offtime has passed
            m_restartFlag = 0; // set the flag back to 0 to avoid restart being called again during update
            return;
        }
        return;
    };

    float getOutputV() { return OutputV; };
    bool getPGOOD() { return PGOOD; };

private:
    float OutputV;
    bool PGOOD;

    // Systemflags
    Types::CoreTypes::SystemStatus_t &m_systemstatus;

    // Pin Definitions
    int8_t m_PGoodPin = -1;
    int8_t m_ENPin = -1;
    bool m_defaultEN = 0;
    bool m_invertEN = 0;
    int8_t m_VReadPin = -1;

    bool m_restartFlag = 0;

    uint32_t m_prevtime;
    uint32_t m_offTime;

    ADC_VRailMonitor m_servoVoltage;
};
