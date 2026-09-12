/* This is a library for the ADS131M06 6-channel ADC

   Product information:
   https://www.ti.com/product/ADS131M06/part-details/ADS131M06IPBSR

   Datasheet:
   https://www.ti.com/lit/ds/symlink/ads131m06.pdf?ts=1688985721879&ref_url=https%253A%252F%252Fduckduckgo.com%252F

   This library was made for Imperial College London Rocketry
   Created by Aleksandr Kent

   Based on ADS131M04-Lib by Daniele Valentino Bella & Iris Clercq-Roques:
   https://github.com/icl-rocketry/ADS131M04-Lib
*/

/*DEV:
  - Add in a command/option to sync devices (or check they are synced). The command SYNC/RESET maybe should be made in the overall code
  - for transferword, can we stick with transmitting 8 bits at a time or should this be changed to 16 bits for commands? (commands are each 16
  bits)
  - function to output array data
  -
*/

#include <Arduino.h>
#include <esp32-hal-ledc.h>
#include <SPI.h>
#include "ADS131M06.h"
#include <libriccore/riccorelogging.h>

ADS131M06::ADS131M06(SPIClass &_spi, uint8_t _csPin, uint8_t _clkoutPin, uint8_t _clockCh):
spi(_spi),
_spisettings(SCLK_SPD, MSBFIRST, SPI_MODE1),
csPin(_csPin),
clkoutPin(_clkoutPin),
clockCh(_clockCh),
clockEnabled(true),
initialised(false)
{}

ADS131M06::ADS131M06(SPIClass &_spi, uint8_t _csPin):
spi(_spi),
_spisettings(SCLK_SPD, MSBFIRST, SPI_MODE1),
csPin(_csPin),
clockEnabled(false),
initialised(false)
{}
//-----------------------------------------------------------------------------------------------------------------------------------------------
void ADS131M06::setup() {
  pinMode(csPin, OUTPUT);//set the pinmode of csPin to output data
  digitalWrite(csPin, HIGH);//set the csPin to output high (active low)

  /* Set CLKOUT on the ESP32. used for generation of the ADC clock signal using ledc.
    Define the individual clock channel to generate the signal and the clock frequency to
    generate it at.
    AttachPin takes the GPIO pin which will be attached to the channel specified.

  */

 if (clockEnabled == true){
  // ledcSetup(clockCh, CLKIN_SPD, 2); //duty cycle resolution = 2bits? check datasheet
  // ledcAttachPin(clkoutPin, clockCh);
  ledcAttachChannel(clkoutPin,CLKIN_SPD,2, 2);
  ledcWrite(clkoutPin, 2);
 }

  setOSR(OSROPT::OSR16256);
  setGain(5, ADS131M06::GAIN::GAIN1);
  setGain(4, ADS131M06::GAIN::GAIN1);
  setGain(3, ADS131M06::GAIN::GAIN1);
  setGain(2, ADS131M06::GAIN::GAIN1);
  setGain(1, ADS131M06::GAIN::GAIN1);
  setGain(0, ADS131M06::GAIN::GAIN1);


  initialised=true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
void ADS131M06::update(){

  rawChannels(outputArray);

}

void ADS131M06::rawChannels(std::array<int32_t,6>& outputArray) {
  /* Writes data from the channels specified in channelArr, to outputArr,
     in the correct order.

     channelArr should have values from 0-5 (specifying the 6 ADC channels), and channelArrLen should be the
     length of that array, starting from 1. (length = 4 if 0-3 channels are used)
     function is void as pointers are used to modify the arrays which are passed into the function.
  */
    spiCommFrame(responseArray);

    // Save the decoded data for each of the channels
    for (int8_t i = 0; i < outputArray.size(); i++) {
        outputArray[i] = twoCompDeco(responseArray[i+1]);
    }

}

// bool ADS131M06::globalChop(bool enabled, uint8_t log2delay) {//XXXXXXXXXXXXXXXXXXXXXXXX
//   /* Function to configure global chop mode for the ADS131M04.

//      INPUTS:
//      enabled - Whether to enable global-chop mode.
//      log2delay   - Base 2 log of the desired delay in modulator clocks periods
//      before measurment begins
//      Possible values are between and including 1 and 16, to give delays
//      between 2 and 65536 (2^16) clock periods respectively
//      For more information, refer to the datasheet.

//      Returns true if settings were written succesfully.
//   */

//   uint8_t delayRegData = log2delay - 1;

//   // Get current settings for current detect mode from the CFG register
//   uint16_t currentDetSett = (readReg(CFG) << 8) >>8;
//   // TRY LOOP HERE INSTEAD?

//   uint16_t newRegData = (delayRegData << 12) + (enabled << 8) + currentDetSett;
//   /*Define the new register data as the sum of a shifted delay data, shifted enable bool var (1 = true, 0=false. these are then shifted)
//   and currentdetsett. More likely on datasheet as this configuration is arbitrary and defines the delay time and whether to set the globalchop
//   as enabled or not*/

//   return writeReg(CFG, newRegData);// returns true is successfull in writing the data to the CFG register.
// }
//-----------------------------------------------------------------------------------------------------------------------------------------------

bool ADS131M06::writeReg(uint8_t reg, uint16_t data) {
  uint8_t commandPref = 0x06;//0000 0110

  // Make command word using syntax found in data sheet
  uint16_t commandWord = (commandPref<<12) + (reg<<7);

  digitalWrite(csPin, LOW);
  spi.beginTransaction(_spisettings);

  // sends command word and data to be written in the register location along the SPI lines. return data is discarded (not assigned to anything)
  spiTransferWord(commandWord);

  /*Use transferWord() function instead of spiCommFrame() since the data needs to be sent directly
  after the command, instead of the 6 empty words being sent after the command in spiCommFrame()*/
  spiTransferWord(data);

  // Send 6 empty words
  for (uint8_t i=0; i<6; i++) {
    spiTransferWord(); //sends 2bytes of zeros by default. completes the 8 word frame
  }

  spi.endTransaction();
  digitalWrite(csPin, HIGH); //active low due to CPHA as mentioned before, therefore deactivate here.

  // Get response in order to determine whether the writing was successfull:
  spiCommFrame(responseArray); // Copy SPI data from all channels into the responseArr[]

  // Determining whether the register was successfully written to or not:
  //response format: 010a aaaa ammm mmmm
  if ( ( (0x04<<12) + (reg<<7) ) == (responseArray[0] >> 16)) { //check against the 16 bit response
    return true;
  } else {
    return false;
  }
}

uint16_t ADS131M06::readReg(uint8_t reg){
  //reg: aaaa aaaa 8 bits, however, no register address requires more than 6 bits other than the RESERVED address (requires 7 bits)
  uint8_t commandPref = 0x0A;//10 = 0000 1010
  //1010 0000 0000 0000
  // Make command word using syntax found in data sheet: 101a aaaa annn nnnn
  uint16_t commandWord = (commandPref << 12) + (reg << 7);

  //Define response array:

  // Use first frame to send command
  spiCommFrame(responseArray, commandWord);

  // Read response: response occurs on the output frame following the command, rather than at the same time the command is sent.
  spiCommFrame(responseArray);//xxxx test if this actuall gets the register data (might just output
  //the channel data, which is not what we want here)
  return (responseArray[0] >> 16); //return the 2bytes of actual data in the first index of the array.

}
//---------------------------------------------------------------------------------------------------------------------------------------------
bool ADS131M06::clkConfig(){
  /*Writes to the clock register and sets crystal oscillator to disable and enable */

  uint16_t data = 0b0011111111001110;
  return(writeReg(CLOCK, data));
}
//-----------------------------------------------------------------------------------------------
bool ADS131M06::reset(){
  uint16_t commandWord = 0x11;

  uint16_t commandResponse = (0xFF <<8) + 0x22; //expected response to be checked against

  spiCommFrame(responseArray, commandWord);
  spiCommFrame(responseArray);

  // check if command was acknowledged:
  if((responseArray[0] >> 16) == commandResponse){ //reponseArr is 32bit MSB aligned, therefore shift 16 and get rid of LSB zeroes.
    return true;
  }else{
    return false;
  }
}

bool ADS131M06::standby(void){
  /*Enter standby mode for the ADC
    output true if successfull
  */
  uint16_t commandWord = 0x20;

  spiCommFrame(responseArray, commandWord);
  spiCommFrame(responseArray);

  // check if command was acknowledged:
  if((responseArray[0] >> 16) == commandWord){//response word is the same as the commandword
    return true;
  }else{
    return false;
  }
}
bool ADS131M06::wakeup(void){
  /*Wakeup from the standby mode
    Output true if successfull*/
  uint16_t commandWord = 0x33; // 0000 0000 0011 0011


  spiCommFrame(responseArray, commandWord);
  spiCommFrame(responseArray);

  // check if command was acknowledged:
  if((responseArray[0] >> 16) == commandWord){//response word is the same as the commandword
    return true;
  }else{
    return false;
  }
}
bool ADS131M06::lock(void){
  /**/
  uint16_t commandWord = (0x05 << 8) + (0x05 << 4) + 0x05; // 0000 0101 0101 0101

  spiCommFrame(responseArray, commandWord);
  spiCommFrame(responseArray);

  // check if command was acknowledged:
  if((responseArray[0] >> 16) == commandWord){//response word is the same as the commandword
    return true;
  }else{
    return false;
  }
}
bool ADS131M06::unlock(void){
  /**/
  uint16_t commandWord = (0x06 << 8) + (0x05 << 4) + 0x05; // 0000 0110 0101 0101

  spiCommFrame(responseArray, commandWord);
  spiCommFrame(responseArray);

  // check if command was acknowledged:
  if((responseArray[0] >> 16) == commandWord){//response word is the same as the commandword
    return true;
  }else{
    return false;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------
uint32_t ADS131M06::spiTransferWord(uint16_t inputData) {

  uint32_t data = spi.transfer(inputData >> 8);

  data <<= 8;

  data |= spi.transfer((inputData<<8) >> 8);

  data <<= 8;

  data |= spi.transfer(0x00);

  return data << 8;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void ADS131M06::spiCommFrame(std::array<uint32_t,8>& commOutput, uint16_t command) {

  digitalWrite(csPin, LOW);

  spi.beginTransaction(_spisettings);

  commOutput[0] = spiTransferWord(command);

  for (uint8_t i=1; i < 7; i++) {//6 times for all ADC channels

    commOutput[i] = spiTransferWord() >> 8;
  }

  commOutput[8] = spiTransferWord(); //CRC data

  spi.endTransaction();

    digitalWrite(csPin, HIGH);
}

bool ADS131M06::setOSR(OSROPT OSR){
    CLOCKREG &= OSRMASK;
    CLOCKREG |= static_cast<uint8_t>(OSR);
    return writeReg(CLOCK,CLOCKREG);
}

bool ADS131M06::setGain(uint8_t channel, GAIN gain){
    uint8_t GainRegAddr;
    uint16_t gainREGValue;
    switch(channel){
      case(0):
      {
        GAIN1REG &= static_cast<uint16_t>(CHGAINMASK::CH04);
        GAIN1REG |= static_cast<uint16_t>(gain);
        GainRegAddr = GAIN1;
        gainREGValue = GAIN1REG;
        break;
      }
      case(1):
      {
        GAIN1REG &= static_cast<uint16_t>(CHGAINMASK::CH15);
        GAIN1REG |= static_cast<uint16_t>(gain) << 4;
        GainRegAddr = GAIN1;
        gainREGValue = GAIN1REG;
        break;
      }
      case(2):
      {
        GAIN1REG &= static_cast<uint16_t>(CHGAINMASK::CH2);
        GAIN1REG |= static_cast<uint16_t>(gain) << 8;
        GainRegAddr = GAIN1;
        gainREGValue = GAIN1REG;
        break;
      }
      case(3):
      {
        GAIN1REG &= static_cast<uint16_t>(CHGAINMASK::CH3);
        GAIN1REG |= static_cast<uint16_t>(gain) << 12;
        GainRegAddr = GAIN1;
        gainREGValue = GAIN1REG;
        break;
      }
      case(4):
      {
        GAIN2REG &= static_cast<uint16_t>(CHGAINMASK::CH04);
        GAIN2REG |= static_cast<uint16_t>(gain);
        GainRegAddr = GAIN2;
        gainREGValue = GAIN2REG;
        break;
      }
      case(5):
      {
        GAIN2REG &= static_cast<uint16_t>(CHGAINMASK::CH15);
        GAIN2REG |= static_cast<uint16_t>(gain) << 4;
        GainRegAddr = GAIN2;
        gainREGValue = GAIN2REG;
        break;
      }
      default:{
          return(false);
      }
    }


    return writeReg(GainRegAddr,gainREGValue);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
int32_t ADS131M06::twoCompDeco(uint32_t data) {//XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  // Take the two's complement of the data
  data <<= 8;
  int32_t dataInt = (int)data;

  return dataInt >> 8;
}