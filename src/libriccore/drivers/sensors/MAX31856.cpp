#include "MAX31856.h"

MAX31856::MAX31856(SPIClass &spi, uint8_t cs, int8_t faultPin, int8_t DRDYPin):
m_spi(spi), m_spisettings(5e6, MSBFIRST, SPI_MODE1), m_cs(cs) {};

bool MAX31856::setup(TCType TCType, OCSet OCSet, AVSet AVSet, ConvModes Mode, ENCJComp EN, FaultModes Fault, FilterFreqs Freq)
{
    clearFault();
    m_init = alive();
    setConversionMode(ConvModes::normOff);
    setTCType(TCType);
    setOCDetection(OCSet);
    setAveraging(AVSet);
    enableCJComp(EN);
    setFaultMode(Fault);
    setFilter(Freq);
    setConversionMode(Mode);
    return m_init;
}

bool MAX31856::alive(){
    for (uint8_t i = 0; i < 16; ++i){ // there are 16 registers that can be read from starting at address 0
        if (readRegister(static_cast<readRegisters>(i),1) != 0){ 
            return true; //If there are any non-zero registers, the device is alive 
        }
    }
    return false;
}

void MAX31856::writeRegister(writeRegisters target, uint8_t data)
{
    m_spi.beginTransaction(m_spisettings);
    digitalWrite(m_cs, LOW);
    m_spi.transfer(static_cast<uint8_t>(target)); //Select the register to write to
    m_spi.transfer(data); //And write data to it
    m_spi.endTransaction();
    digitalWrite(m_cs, HIGH);
}

uint32_t MAX31856::readRegister(readRegisters target, uint8_t Nbytes)
{
    uint32_t regData = 0;
    m_spi.beginTransaction(m_spisettings);
    digitalWrite(m_cs, LOW);
    m_spi.transfer(static_cast<uint8_t>(target)); //Select register to read from
    regData = m_spi.transfer(0x00);
    //In the MAX31856, reading more data without sending a new register reads from the next register address.
    for (uint8_t i = 0; i < Nbytes - 1; ++i) 
    {
        regData <<= 8;
        regData |= m_spi.transfer(0x00);
    }
    m_spi.endTransaction();
    digitalWrite(m_cs, HIGH);
    return regData;
}

void MAX31856::setParameter(writeRegisters Addr, uint8_t& ShadowReg, uint8_t Mask, uint8_t Parameter)
{
    ShadowReg &= Mask; 
    ShadowReg |= Parameter; 
    writeRegister(Addr, ShadowReg);
}

bool MAX31856::checkFault(Faults Fault){
    if (m_activeFaults & Fault){ //Check active faults with the fault mask. Will return an integer larger than 0 if the fault is asserted.
        return true;
    }
    else {
        return false;
    }
}

void MAX31856::update()
{
    if (!m_init){ //Don't udpate if device failed to initialise
        return;
    }
    // if(millisFast() - m_CJtimerOFF > 60000 && _ColdJComp == ENCJComp::disable){
    //     enableCJComp(ENCJComp::enable);
    //     m_CJtimerON = millisFast();
    // }

    // if(millisFast() - m_CJtimerON > 200 && _ColdJComp == ENCJComp::disable){
    //     enableCJComp(_ColdJComp);
    //     m_CJtimerOFF = millisFast();
    // }
    
    if (millisFast() - m_prevUpdate > m_updateDelta)
    {
        m_activeFaults = static_cast<Faults>(readRegister(readRegisters::Fault,1));
        if(checkFault(OPEN) || checkFault(OVUV)){ //If thermocouple is open circuit or if the device is in OV/UV protection, the temp value can't be trusted
            m_Temp = NAN;
            return;
        }
        int32_t TempReg = readRegister(readRegisters::LinTempB2, 3);

        if (TempReg & SignMask24Bit)
        {
            TempReg |= 0xFF000000;
        }
        m_Temp = (float)TempReg * fpScale;

        if (m_Temp < -250)
        {
            m_Temp = NAN;
        }
        m_prevUpdate = millisFast();
    }
}

void MAX31856::clearFault()
{
    setParameter(writeRegisters::Config0, C0Reg, FaultClearMask, 0b00000010);
}

void MAX31856::setTCType(TCType TCType)
{
    setParameter(writeRegisters::Config1, C1Reg, TypeMask, TCType);
}

void MAX31856::setOCDetection(OCSet OCSet)
{
    setParameter(writeRegisters::Config0, C0Reg, OCFaultMask, OCSet);
}

void MAX31856::setAveraging(AVSet AVset)
{
    setConversionMode(ConvModes::normOff); //Some settings need to be updated in the normally off mode
    setParameter(writeRegisters::Config1, C1Reg, AVMask, AVset);
    setConversionMode(_ConvMode);
}

void MAX31856::setConversionMode(ConvModes Mode)
{
    setParameter(writeRegisters::Config0, C0Reg, ModeMask, Mode);
}

void MAX31856::enableCJComp(ENCJComp EN)
{
    setParameter(writeRegisters::Config0, C0Reg, CJCompENMask, EN);
}

void MAX31856::setFaultMode(FaultModes Mode)
{
    setParameter(writeRegisters::Config0, C0Reg, FaultModeMask, Mode);
}

void MAX31856::setFilter(FilterFreqs Freq)
{
    setConversionMode(ConvModes::normOff); //Some settings need to be updated in the normally off mode
    setParameter(writeRegisters::Config0, C0Reg, FilterMask, Freq);
    setConversionMode(_ConvMode);
}

void MAX31856::setCJOffset(float CJOffset)
{
    uint8_t CJOff = CJOffset; //TODO Change this so it gets converted into the format expected by the max31856
    setParameter(writeRegisters::CJTOffset, CJOffReg, 0, CJOff);
}