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


#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <array>

class ADS131M06 {
  public:
    ADS131M06(SPIClass &_spi, uint8_t _csPin, uint8_t _clkoutPin, uint8_t _clockCh = 1);
    ADS131M06(SPIClass &_spi, uint8_t _csPin); //second ADC uses the same master clock setup by first ADC
    /**
     * @brief Setup function of the ADC. Configure the Cs pin, begin SPI and set Cs to high
     * (active low). Configure the clock source options for the ADC using clkConfig() and setup
     * ledc frequency and pin option
     *
     */
    void setup();

    /**
     * @brief update method. Called every time the system updates in the loop. Use to
     * serial print the data values of the ADCs
     *
     */
    void update();


    /**
     * @brief Writes data from the channels specified in channelArr, to outputArr,
     * in the correct order (order of channels specified from channelArr).
     * Copies data from all the channels into a rawDataArr created in the function using
     * spicommframe(), then loops through the array
     * and only copies the data from the channels specified in channelArr. Uses twocompDeco to change data format.
     *
     * @param channelArrPtr channelArr should have values from 0-5 (specifying the 6 ADC channels) in any order required
     * @param channelArrLen channelArrLen should be the length of that array, starting from 1.
     *                      (length = 4 if 0-3 channels are used)
     * @param outputArrPtr Must match length of channelArrPtr
     */
    void rawChannels(std::array<int32_t, 6>& outputArray);

    /**
     * @brief Returns raw data value from the channel specified. Wrapper function of rawChannels() function
     * where the length of the chanelArr and outputArr are 1. The arrlength identified is also 1.
     *
     * @param channel zero indexed value (same as values in channelArrPtr)
     * @return int32_t
     */
    int32_t rawChannelSingle(int8_t channel);

    /**
     * @brief Reads the data at the register specified as reg. Can read 1 individual register or multiple (if a number
     * greater than zero is specified for number).
     *
     * @param reg
     * //param number The number of consecutive registers to read from (not zero indexed)xxxxxx
     * @return uint16_t (arrptr to the array containing the data). Data is in first index if only 1 register
     *         is read. If multiple, then the first index is the acknowledgement and the data are in the consecutive
     *         indexes.
     */
    uint16_t readReg(uint8_t reg);//modify for multiple consecutive register useage

    /**
     * @brief writes to the specified register address, replacing the old register data with the
     * new data provided. returns true if process completed successfully.
     *
     * @param reg
     * @param data
     * @param number
     * @return true
     * @return false
     */
    bool writeReg(uint8_t reg, uint16_t data);//modify for multiple consecutive

    /**
     * @brief Configures the clock generation options in the ADC by disabling the internal
     * crystal oscillator and enabling the external clock reference option. Output true if
     * operation is successfull.
     *
     * @return true
     * @return false
     */
    bool clkConfig();

    //commands:-------------------------------------------------------------------------------
    bool reset();//xxxxxx
    bool standby();//xxxxxxxx
    bool wakeup();//xxxxxx
    bool lock();//xxxxxxxxx
    bool unlock();//xxxxxxxx
    //------------------------------------------------------------------------------------------

    bool globalChop(bool enabled = false, uint8_t log2delay = 4);

    int32_t getOutput(uint8_t ch){return outputArray[ch];};

    enum class OSROPT : uint16_t{
        OSR128 = 0b00000,
        OSR256 = 0b00100,
        OSR512 = 0b01000,
        OSR1024 = 0b01100,
        OSR2048 = 0b10000,
        OSR4096 = 0b10100,
        OSR8192 = 0b11000,
        OSR16256 = 0b11100
    };

    enum class GAIN : uint16_t{
        GAIN1 = 0b000,
        GAIN2 = 0b001,
        GAIN4 = 0b010,
        GAIN8 = 0b011,
        GAIN16 = 0b100,
        GAIN32 = 0b101,
        GAIN64 = 0b110,
        GAIN128 = 0b111
    };

    enum class CHGAINMASK : uint16_t{
        CH04 = 0b1111111111111000,
        CH15 = 0b1111111110001111,
        CH2 = 0b1111100011111111,
        CH3 = 0b1000111111111000,
    };

    bool setOSR(OSROPT OSR);
    bool setGain(uint8_t channel, GAIN gain);

  private:

    SPIClass& spi;
    SPISettings _spisettings;
    uint8_t csPin, clkoutPin, clockCh;
    bool clockEnabled;
    bool initialised;
    std::array<uint32_t,6> resultsArray;
    std::array<uint32_t,8> responseArray;
    std::array<int32_t,6> outputArray;

    /**
     * @brief Saves all channel data (32 bits) to the outputArray of size 8 where the first index is the result of the
     * optional 16 bit command (result is 32 bits), the next 6 indexes are the channel data, and the last index (8th)
     * contains the CRC bits. The spiCommFrame() function actually sends an 8 word command and
     * outputs the response in the array specified.
     * Sends an 8 word frame. The first word is the optional command (if this is
     * not chosen, this is the NULL command by default, which gives the content of the status
     * register).
     *
     * @param outputArray Length 8
     * @param command optional 16 bit command
     */
    void spiCommFrame(std::array<uint32_t,8>& outputArray, uint16_t command = 0x0000);

    /**
     * @brief Transfer a 24 bit word, which is the result of concatinating 16 bits of input data (first 2 bytes) with
     * a byte of zeros 0x00. Data is sent a byte at a time with the MSB sent first. Data returned is thus MSB aligned
     * concatinated into a 32bit result in the format: MSB_response, LSB_response, zero_response, zeroPadding. Each
     * is a byte of data, therefore 32 bits in total.
     * Default data to send are zeros
     *
     * @param inputData 16 bits
     * @return uint32_t
     */
    uint32_t spiTransferWord(uint16_t inputData = 0x0000);

    /**
     * @brief
     *
     * @param data
     * @return int32_t
     */
    int32_t twoCompDeco(uint32_t data);

    uint16_t GAIN1REG = 0x0000;
    uint16_t GAIN2REG = 0x0000;
    static constexpr int32_t Mask24bit = 0b111111111111111111111111;
    static constexpr int32_t SignMask24bit = 0b100000000000000000000000;

    //Settings
    static constexpr uint16_t OSRMASK = 0b1111111111100011;

    uint16_t CLOCKREG = 0b0011111100001110; //Default settings

    //Register definitions:-------------------------------------------------------------------------
    static constexpr uint8_t ID = 0x00;
    static constexpr uint8_t STATUS = 0x01;
    static constexpr uint8_t MODE = 0x02;
    static constexpr uint8_t CLOCK = 0x03;
    static constexpr uint8_t GAIN1 = 0x4;
    static constexpr uint8_t GAIN2 = 0x5;
    static constexpr uint8_t CFG = 0x06;
    static constexpr uint8_t THRSHLD_MSB = 0x07;
    static constexpr uint8_t THRSHLD_LSB = 0x08;

    static constexpr uint32_t CLKIN_SPD = 8.192e6; // Clock speed (Hz) for the CLKIN on the LEDC xxxx
    static constexpr uint32_t SCLK_SPD = 25e6; // SPI transaction frequency xxxxxxxx

    //channels: config, ... most sig bit, ... least sig bit,
    static constexpr uint8_t CH0_CFG = 0x09;
    static constexpr uint8_t CH0_OCAL_MSB = 0x0A;
    static constexpr uint8_t CH0_OCAL_LSB = 0x0B;
    static constexpr uint8_t CH0_GCAL_MSB = 0x0C;
    static constexpr uint8_t CH0_GCAL_LSB = 0x0D;

    static constexpr uint8_t CH1_CFG = 0x0E;
    static constexpr uint8_t CH1_OCAL_MSB = 0x0F;
    static constexpr uint8_t CH1_OCAL_LSB = 0x10;
    static constexpr uint8_t CH1_GCAL_MSB = 0x11;
    static constexpr uint8_t CH1_GCAL_LSB = 0x12;

    static constexpr uint8_t CH2_CFG = 0x13;
    static constexpr uint8_t CH2_OCAL_MSB = 0x14;
    static constexpr uint8_t CH2_OCAL_LSB = 0x15;
    static constexpr uint8_t CH2_GCAL_MSB = 0x16;
    static constexpr uint8_t CH2_GCAL_LSB = 0x17;

    static constexpr uint8_t CH3_CFG = 0x18;
    static constexpr uint8_t CH3_OCAL_MSB = 0x19;
    static constexpr uint8_t CH3_OCAL_LSB = 0x1A;
    static constexpr uint8_t CH3_GCAL_MSB = 0x1B;
    static constexpr uint8_t CH3_GCAL_LSB = 0x1C;

    static constexpr uint8_t CH4_CFG = 0x1D;
    static constexpr uint8_t CH4_OCAL_MSB = 0x1E;
    static constexpr uint8_t CH4_OCAL_LSB = 0x1F;
    static constexpr uint8_t CH4_GCAL_MSB = 0x20;
    static constexpr uint8_t CH4_GCAL_LSB = 0x21;

    static constexpr uint8_t CH5_CFG = 0x22;
    static constexpr uint8_t CH5_OCAL_MSB = 0x23;
    static constexpr uint8_t CH5_OCAL_LSB = 0x24;
    static constexpr uint8_t CH5_GCAL_MSB = 0x25;
    static constexpr uint8_t CH5_GCAL_LSB = 0x26;

    static constexpr uint8_t REGMAP_CRC = 0x3E;
    static constexpr uint8_t RESERVED = 0x3F; //This register is exempt from read reg for ADC
};