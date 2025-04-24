/**
 * @file MAX31856.h
 * @author Andrei Paduraru (ap2621@ic.ac.uk)
 * @brief Class to interact with MAX31856 thermocouple ICs.
 * @version 0.2
 * @date 2024-03-1
 * 
 * @copyright Copyright (c) 2023-2024
 * 
 */

#pragma once

#include <Arduino.h>
#include <SPI.h>

class MAX31856{

    public:
        //Enums to make device settings readable.
        //This is only here because arduino requires enums to be defined before they're used as a function input.
        enum TCType : uint8_t {
            TB = 0b00000000,
            TE = 0b00000001,
            TJ = 0b00000010,
            TK = 0b00000011,
            TN = 0b00000100,
            TR = 0b00000101,
            TS = 0b00000110,
            TT = 0b00000111
        };

        enum OCSet : uint8_t {
            disabled = 0,
            RSL5K = 0b00010000,
            RSH5KTCL2 = 0b00100000,
            RSH5KTCH2 = 0b00110000
        };
        
        enum AVSet : uint8_t {
            onesamp = 0,
            twosamp = 0b00010000,
            foursamp = 0b00100000,
            eightsamp = 0b00110000,
            sixteensamp = 0b01000000
        };

        enum ConvModes : uint8_t {
            normOff = 0,
            conti = 0b10000000
        };

        enum ENCJComp : uint8_t {
            enable = 0,
            disable = 0b00001000
        };

        enum FaultModes : uint8_t {
            comparator = 0,
            interrupt = 0b00000100
        };

        enum FilterFreqs : uint8_t {
            sixtyHz = 0b00000001,
            fiftyHz = 0
        };

        enum Faults : uint8_t {
            CJRange = (1<<7),
            TCRange = (1<<6),
            CJHigh = (1<<5),
            CJLow = (1<<4),
            TCHigh = (1<<3),
            TCLow = (1<<2),
            OVUV = (1<<1),
            OPEN = (1<<0),
        };
        
        MAX31856(SPIClass &spi, uint8_t cs, int8_t faultPin = -1, int8_t DRDYPin = -1);
        bool setup(TCType TCType = TCType::TK, OCSet OCSet = OCSet::RSL5K, AVSet AVSet = AVSet::onesamp, ConvModes Mode = ConvModes::conti, ENCJComp EN = ENCJComp::enable, FaultModes Fault = FaultModes::comparator, FilterFreqs Freq = FilterFreqs::sixtyHz); //TODO modify to take in a struct and return a bool for whether setup was successful.
        void reset(); //TODO implement
        void setTCType(TCType TCType);
        void setOCDetection(OCSet OCSet);
        void setAveraging(AVSet AVset);
        void setConversionMode(ConvModes Mode);
        void enableCJComp(ENCJComp EN);
        void setFaultMode(FaultModes Mode);
        void setFilter(FilterFreqs Freq);
        void setCJOffset(float CJT);
        void maskFault(Faults FaultToMask); //TODO implement
        float getTemp() {return m_Temp;};
        /**
         * @brief Getter for device fault flags that are raised.
         * @return Faults encoded in a bitfield, which is described by the "Fault" enum.
        */
        uint8_t getFault() {return m_activeFaults;};
        /**
         * @brief Checks if "Fault" is raised.
         * @return True if the fault is raised, false otherwise.
        */
        bool checkFault(Faults Fault);

        void update();
        
    private:

        enum class readRegisters : uint8_t {
            Config0 = 0x00,
            Config1 = 0x01,
            FaultMask = 0x02,
            CJHFault = 0x03,
            CJLFault = 0x04,
            LinTempFltHMSB = 0x05,
            LinTempFlttHLSB = 0x06,
            LinTempFltLMSB = 0x07,
            LinTempFltLLSB = 0x08,
            CJTOffset = 0x09,
            CJTH = 0x0A,
            CJTL = 0x0B,
            LinTempB2 = 0x0C,
            LinTempB1 = 0x0D,
            LinTempB0 = 0x0E,
            Fault = 0x0F 
        };

        enum class writeRegisters : uint8_t {
            Config0 = 0x80,
            Config1 = 0x81,
            FaultMask = 0x82,
            CJHFault = 0x83,
            CJLFault = 0x84,
            LinTempFltHMSB = 0x85,
            LinTempFlttHLSB = 0x86,
            LinTempFltLMSB = 0x87,
            LinTempFltLLSB = 0x88,
            CJTOffset = 0x89,
            CJTH = 0x8A,
            CJTL = 0x8B,
        };
        
        uint32_t readRegister(readRegisters target, uint8_t Nbytes);
        void writeRegister(writeRegisters target, uint8_t data);
        void setParameter(writeRegisters RegisterAddr, uint8_t& ShadowRegister, uint8_t Mask, uint8_t Parameter);
        /**
         * @brief Identify if the device is alive. Note, this device doesn't have a whoami register, so this is only implemented as check if any registers are non-zero, which should be the case because some registers reset to a non-zero value.
         * @return True if the device is communicating, false otherwise.
        */
        bool alive();
        void clearFault();

        //Fast millis bcs why not
        uint32_t millisFast(){
            return xTaskGetTickCount() * portTICK_PERIOD_MS;
        };

        //Initialising
        float m_Temp = 0;
        uint32_t m_prevUpdate = 0;
        uint16_t m_updateDelta = 41;
        bool m_init = false;
        Faults m_activeFaults = static_cast<Faults>(0); //Initialised to 0, i.e. no faults active.

        //SPI adjacent settings
        SPIClass& m_spi;
        SPISettings m_spisettings;
        uint8_t m_cs;

        //Settings
        TCType _Type;
        OCSet _OpenCircDet;
        AVSet _Averaging;
        ConvModes _ConvMode;
        ENCJComp _ColdJComp;
        FaultModes _FMode;
        FilterFreqs _Freq;

        //Shadow registers, all set to factory defaults
        uint8_t C0Reg = 0x00;
        uint8_t C1Reg = 0x03;
        uint8_t CJOffReg = 0x00;

        //Magic numbers
        static constexpr float fpScale = 1.0/4096.0; //floating point conversion scale to convert a temperature stored in binary to a float.
    
        //General masks
        static constexpr uint32_t SignMask24Bit = 0b100000000000000000000000;
        static constexpr uint32_t MaskAll24Bits = 0b111111111111111111111111;
        
        //Masks for CONFIG0 register options
        static constexpr uint8_t ModeMask = 0b01111111;
        static constexpr uint8_t OCFaultMask = 0b11001111;
        static constexpr uint8_t CJCompENMask = 0b11110111;
        static constexpr uint8_t FaultModeMask = 0b11111011;
        static constexpr uint8_t FaultClearMask = 0b11111101;
        static constexpr uint8_t FilterMask = 0b11111110;

        //Masks for CONFIG1 register options
        static constexpr uint8_t TypeMask = 0b11110000;
        static constexpr uint8_t AVMask = 0b10001111;

        uint32_t m_CJtimerOFF = 0;
        uint32_t m_CJtimerON = 0;

};