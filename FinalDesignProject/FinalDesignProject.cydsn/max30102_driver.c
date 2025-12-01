/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include "max30102_driver.h"
#include "stdbool.h"

sense_struct sense;
uint8_t activeLEDs = 2;  // Default to Red + IR for SpO2
uint8_t revisionID = 0;

// Low-level I2C Functions
uint8_t MAX30105_readRegister8(uint8_t reg) {
    uint8_t data = 0;
    
    I2C_I2CMasterSendStart(MAX30105_I2C_ADDR, 0, MAX30105_TIMEOUT);
    I2C_I2CMasterWriteByte(reg, MAX30105_TIMEOUT);
    I2C_I2CMasterSendRestart(MAX30105_I2C_ADDR, 1, MAX30105_TIMEOUT);
    I2C_I2CMasterReadByte(I2C_I2C_NAK_DATA, &data, MAX30105_TIMEOUT);
    I2C_I2CMasterSendStop(MAX30105_TIMEOUT);
    
    return data;
}

void MAX30105_writeRegister8(uint8_t reg, uint8_t value) {
    I2C_I2CMasterSendStart(MAX30105_I2C_ADDR, 0, MAX30105_TIMEOUT);
    I2C_I2CMasterWriteByte(reg, MAX30105_TIMEOUT);
    I2C_I2CMasterWriteByte(value, MAX30105_TIMEOUT);
    I2C_I2CMasterSendStop(MAX30105_TIMEOUT);
}

void MAX30105_bitMask(uint8_t reg, uint8_t mask, uint8_t thing) {
    uint8_t originalContents = MAX30105_readRegister8(reg);
    originalContents = originalContents & mask;
    MAX30105_writeRegister8(reg, originalContents | thing);
}

// Initialization
bool MAX30105_begin(void) {
    // Check that a MAX30105 is connected
    if (MAX30105_readPartID() != MAX30105_EXPECTEDPARTID) {
        return false;
    }
    
    MAX30105_readRevisionID();
    return true;
}

// Device ID
uint8_t MAX30105_readPartID(void) {
    return MAX30105_readRegister8(MAX30105_PARTID);
}

void MAX30105_readRevisionID(void) {
    revisionID = MAX30105_readRegister8(MAX30105_REVISIONID);
}

uint8_t MAX30105_getRevisionID(void) {
    return revisionID;
}

// Configuration Functions
void MAX30105_softReset(void) {
    MAX30105_bitMask(MAX30105_MODECONFIG, MAX30105_RESET_MASK, MAX30105_RESET);
    
    // Wait for reset to complete (max 100ms)
    uint32_t startTime = 0;
    while (startTime < 100) {
        uint8_t response = MAX30105_readRegister8(MAX30105_MODECONFIG);
        if ((response & MAX30105_RESET) == 0) break;
        CyDelay(1);
        startTime++;
    }
}

void MAX30105_shutDown(void) {
    MAX30105_bitMask(MAX30105_MODECONFIG, MAX30105_SHUTDOWN_MASK, MAX30105_SHUTDOWN);
}

void MAX30105_wakeUp(void) {
    MAX30105_bitMask(MAX30105_MODECONFIG, MAX30105_SHUTDOWN_MASK, MAX30105_WAKEUP);
}

void MAX30105_setLEDMode(uint8_t mode) {
    MAX30105_bitMask(MAX30105_MODECONFIG, MAX30105_MODE_MASK, mode);
}

void MAX30105_setADCRange(uint8_t adcRange) {
    MAX30105_bitMask(MAX30105_PARTICLECONFIG, MAX30105_ADCRANGE_MASK, adcRange);
}

void MAX30105_setSampleRate(uint8_t sampleRate) {
    MAX30105_bitMask(MAX30105_PARTICLECONFIG, MAX30105_SAMPLERATE_MASK, sampleRate);
}

void MAX30105_setPulseWidth(uint8_t pulseWidth) {
    MAX30105_bitMask(MAX30105_PARTICLECONFIG, MAX30105_PULSEWIDTH_MASK, pulseWidth);
}

void MAX30105_setPulseAmplitudeRed(uint8_t amplitude) {
    MAX30105_writeRegister8(MAX30105_LED1_PULSEAMP, amplitude);
}

void MAX30105_setPulseAmplitudeIR(uint8_t amplitude) {
    MAX30105_writeRegister8(MAX30105_LED2_PULSEAMP, amplitude);
}

void MAX30105_setPulseAmplitudeGreen(uint8_t amplitude) {
    MAX30105_writeRegister8(MAX30105_LED3_PULSEAMP, amplitude);
}

void MAX30105_setFIFOAverage(uint8_t numberOfSamples) {
    MAX30105_bitMask(MAX30105_FIFOCONFIG, MAX30105_SAMPLEAVG_MASK, numberOfSamples);
}

void MAX30105_enableFIFORollover(void) {
    MAX30105_bitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_ENABLE);
}

void MAX30105_clearFIFO(void) {
    MAX30105_writeRegister8(MAX30105_FIFOWRITEPTR, 0);
    MAX30105_writeRegister8(MAX30105_FIFOOVERFLOW, 0);
    MAX30105_writeRegister8(MAX30105_FIFOREADPTR, 0);
}

void MAX30105_enableSlot(uint8_t slotNumber, uint8_t device) {
    switch (slotNumber) {
        case 1:
            MAX30105_bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT1_MASK, device);
            break;
        case 2:
            MAX30105_bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT2_MASK, device << 4);
            break;
        case 3:
            MAX30105_bitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT3_MASK, device);
            break;
        case 4:
            MAX30105_bitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT4_MASK, device << 4);
            break;
        default:
            break;
    }
}

uint8_t MAX30105_getWritePointer(void) {
    return MAX30105_readRegister8(MAX30105_FIFOWRITEPTR);
}

uint8_t MAX30105_getReadPointer(void) {
    return MAX30105_readRegister8(MAX30105_FIFOREADPTR);
}

// Setup with good defaults for heart rate and SpO2
void MAX30105_setup(uint8_t powerLevel, uint8_t sampleAverage, uint8_t ledMode,
                    int sampleRate, int pulseWidth, int adcRange) {
    MAX30105_softReset();
    
    // FIFO Configuration
    if (sampleAverage == 1) MAX30105_setFIFOAverage(MAX30105_SAMPLEAVG_1);
    else if (sampleAverage == 2) MAX30105_setFIFOAverage(MAX30105_SAMPLEAVG_2);
    else if (sampleAverage == 4) MAX30105_setFIFOAverage(MAX30105_SAMPLEAVG_4);
    else if (sampleAverage == 8) MAX30105_setFIFOAverage(MAX30105_SAMPLEAVG_8);
    else if (sampleAverage == 16) MAX30105_setFIFOAverage(MAX30105_SAMPLEAVG_16);
    else if (sampleAverage == 32) MAX30105_setFIFOAverage(MAX30105_SAMPLEAVG_32);
    else MAX30105_setFIFOAverage(MAX30105_SAMPLEAVG_4);
    
    MAX30105_enableFIFORollover();
    
    // Mode Configuration
    if (ledMode == 3) MAX30105_setLEDMode(MAX30105_MODE_MULTILED);
    else if (ledMode == 2) MAX30105_setLEDMode(MAX30105_MODE_REDIRONLY);
    else MAX30105_setLEDMode(MAX30105_MODE_REDONLY);
    activeLEDs = ledMode;
    
    // Particle Sensing Configuration
    if (adcRange < 4096) MAX30105_setADCRange(MAX30105_ADCRANGE_2048);
    else if (adcRange < 8192) MAX30105_setADCRange(MAX30105_ADCRANGE_4096);
    else if (adcRange < 16384) MAX30105_setADCRange(MAX30105_ADCRANGE_8192);
    else MAX30105_setADCRange(MAX30105_ADCRANGE_16384);
    
    if (sampleRate < 100) MAX30105_setSampleRate(MAX30105_SAMPLERATE_50);
    else if (sampleRate < 200) MAX30105_setSampleRate(MAX30105_SAMPLERATE_100);
    else if (sampleRate < 400) MAX30105_setSampleRate(MAX30105_SAMPLERATE_200);
    else if (sampleRate < 800) MAX30105_setSampleRate(MAX30105_SAMPLERATE_400);
    else if (sampleRate < 1000) MAX30105_setSampleRate(MAX30105_SAMPLERATE_800);
    else if (sampleRate < 1600) MAX30105_setSampleRate(MAX30105_SAMPLERATE_1000);
    else if (sampleRate < 3200) MAX30105_setSampleRate(MAX30105_SAMPLERATE_1600);
    else MAX30105_setSampleRate(MAX30105_SAMPLERATE_3200);
    
    if (pulseWidth < 118) MAX30105_setPulseWidth(MAX30105_PULSEWIDTH_69);
    else if (pulseWidth < 215) MAX30105_setPulseWidth(MAX30105_PULSEWIDTH_118);
    else if (pulseWidth < 411) MAX30105_setPulseWidth(MAX30105_PULSEWIDTH_215);
    else MAX30105_setPulseWidth(MAX30105_PULSEWIDTH_411);
    
    // LED Pulse Amplitude
    MAX30105_setPulseAmplitudeRed(powerLevel);
    MAX30105_setPulseAmplitudeIR(powerLevel);
    if (ledMode > 2) MAX30105_setPulseAmplitudeGreen(powerLevel);
    
    // Multi-LED Mode Configuration
    MAX30105_enableSlot(1, SLOT_RED_LED);
    if (ledMode > 1) MAX30105_enableSlot(2, SLOT_IR_LED);
    if (ledMode > 2) MAX30105_enableSlot(3, SLOT_GREEN_LED);
    
    MAX30105_clearFIFO();
}

// Data Collection Functions
uint8_t MAX30105_available(void) {
    int8_t numberOfSamples = sense.head - sense.tail;
    if (numberOfSamples < 0) numberOfSamples += STORAGE_SIZE;
    return (uint8_t)numberOfSamples;
}

uint32_t MAX30105_getRed(void) {
    if (MAX30105_safeCheck(250))
        return sense.red[sense.head];
    else
        return 0;
}

uint32_t MAX30105_getIR(void) {
    if (MAX30105_safeCheck(250))
        return sense.IR[sense.head];
    else
        return 0;
}

uint32_t MAX30105_getGreen(void) {
    if (MAX30105_safeCheck(250))
        return sense.green[sense.head];
    else
        return 0;
}

uint32_t MAX30105_getFIFORed(void) {
    return sense.red[sense.tail];
}

uint32_t MAX30105_getFIFOIR(void) {
    return sense.IR[sense.tail];
}

uint32_t MAX30105_getFIFOGreen(void) {
    return sense.green[sense.tail];
}

void MAX30105_nextSample(void) {
    if (MAX30105_available()) {
        sense.tail++;
        sense.tail %= STORAGE_SIZE;
    }
}

uint8_t i2c_read8(uint8_t ack)
{
    uint8_t data = 0;
    uint32_t status;

    status = I2C_I2CMasterReadByte(ack, &data, MAX30105_TIMEOUT);
    if (status != I2C_I2C_MSTR_NO_ERROR)
        return 0; // Fail safely
    
    return data;
}

uint16_t MAX30105_check(void) {
    uint8_t readPointer = MAX30105_getReadPointer();
    uint8_t writePointer = MAX30105_getWritePointer();
    
    int numberOfSamples = 0;
    
    if (readPointer == writePointer) return 0;
    
    numberOfSamples = writePointer - readPointer;
    if (numberOfSamples < 0)
        numberOfSamples += 32;
    
    int bytesLeftToRead = numberOfSamples * activeLEDs * 3;
    
    if (I2C_I2CMasterSendStart(MAX30105_I2C_ADDR, I2C_I2C_WRITE_XFER_MODE, MAX30105_TIMEOUT) 
        != I2C_I2C_MSTR_NO_ERROR) return 0;

    if (I2C_I2CMasterWriteByte(MAX30105_FIFODATA, MAX30105_TIMEOUT) 
        != I2C_I2C_MSTR_NO_ERROR) return 0;

    if (I2C_I2CMasterSendRestart(MAX30105_I2C_ADDR, I2C_I2C_READ_XFER_MODE, MAX30105_TIMEOUT) 
        != I2C_I2C_MSTR_NO_ERROR) return 0;

    // ------------------------------------------------------
    // Burst read each sample
    // ------------------------------------------------------
    while (bytesLeftToRead > 0)
    {
        sense.head++;
        sense.head %= STORAGE_SIZE;
        
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;

        // ------------------------------
        // RED (always present)
        // ------------------------------
        b1 = i2c_read8(I2C_I2C_ACK_DATA);
        b2 = i2c_read8(I2C_I2C_ACK_DATA);
        b3 = i2c_read8((bytesLeftToRead == 3) ? I2C_I2C_NAK_DATA : I2C_I2C_ACK_DATA);
        
         uint32_t redLong = ((uint32_t)b1 << 16) |
                           ((uint32_t)b2 << 8)  |
                            b3;
        redLong &= 0x3FFFF;
        sense.red[sense.head] = redLong;

        bytesLeftToRead -= 3;
        if (bytesLeftToRead <= 0) break;
        /*
        tempLong = 0;

        I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, &b, MAX30105_TIMEOUT);
        tempLong |= ((uint32_t)b << 16);

        I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, &b, MAX30105_TIMEOUT);
        tempLong |= ((uint32_t)b << 8);

        // Determine if this is the last byte of the entire read
        uint8_t finalByte = (bytesLeftToRead == 1);

        I2C_I2CMasterReadByte(finalByte ? I2C_I2C_NAK_DATA : I2C_I2C_ACK_DATA,
                               &b, MAX30105_TIMEOUT);
        tempLong |= b;

        tempLong &= 0x3FFFF;
        sense.red[sense.head] = tempLong;

        bytesLeftToRead -= 3;
        if (bytesLeftToRead <= 0) break;
        */

        // ------------------------------
        // IR (if enabled)
        // ------------------------------
        if (activeLEDs > 1)
        {
            b1 = i2c_read8(I2C_I2C_ACK_DATA);
            b2 = i2c_read8(I2C_I2C_ACK_DATA);
            b3 = i2c_read8((bytesLeftToRead == 3) ? I2C_I2C_NAK_DATA : I2C_I2C_ACK_DATA);

            uint32_t irLong = ((uint32_t)b1 << 16) |
                              ((uint32_t)b2 << 8)  |
                               b3;
            irLong &= 0x3FFFF;
            sense.IR[sense.head] = irLong;

            bytesLeftToRead -= 3;
            if (bytesLeftToRead <= 0) break;
        }
        
        /*
        if (activeLEDs > 1)
        {
            tempLong = 0;

            I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, &b, MAX30105_TIMEOUT);
            tempLong |= ((uint32_t)b << 16);

            I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, &b, MAX30105_TIMEOUT);
            tempLong |= ((uint32_t)b << 8);

            finalByte = (bytesLeftToRead == 1);

            I2C_I2CMasterReadByte(finalByte ? I2C_I2C_NAK_DATA : I2C_I2C_ACK_DATA,
                                   &b, MAX30105_TIMEOUT);
            tempLong |= b;

            tempLong &= 0x3FFFF;
            sense.IR[sense.head] = tempLong;

            bytesLeftToRead -= 3;
            if (bytesLeftToRead <= 0) break;
        }
        */

        // ------------------------------
        // GREEN (if enabled)
        // ------------------------------
        if (activeLEDs > 2)
        {
            b1 = i2c_read8(I2C_I2C_ACK_DATA);
            b2 = i2c_read8(I2C_I2C_ACK_DATA);
            b3 = i2c_read8((bytesLeftToRead == 3) ? I2C_I2C_NAK_DATA : I2C_I2C_ACK_DATA);

            uint32_t gLong = ((uint32_t)b1 << 16) |
                             ((uint32_t)b2 << 8)  |
                              b3;
            gLong &= 0x3FFFF;
            sense.green[sense.head] = gLong;

            bytesLeftToRead -= 3;
        }
        
        /*
        if (activeLEDs > 2)
        {
            tempLong = 0;

            I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, &b, MAX30105_TIMEOUT);
            tempLong |= ((uint32_t)b << 16);

            I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, &b, MAX30105_TIMEOUT);
            tempLong |= ((uint32_t)b << 8);

            finalByte = (bytesLeftToRead == 1);

            I2C_I2CMasterReadByte(finalByte ? I2C_I2C_NAK_DATA : I2C_I2C_ACK_DATA,
                                   &b, MAX30105_TIMEOUT);
            tempLong |= b;

            tempLong &= 0x3FFFF;
            sense.green[sense.head] = tempLong;

            bytesLeftToRead -= 3;
        */
        }

    I2C_I2CMasterSendStop(MAX30105_TIMEOUT);

    return (uint16_t)numberOfSamples;
}

bool MAX30105_safeCheck(uint8_t maxTimeToCheck) {
    uint32_t startTime = 0;
    
    while (startTime < maxTimeToCheck) {
        if (MAX30105_check() > 0) {
            return true;
        }
        CyDelay(1);
        startTime++;
    }
    
    return false;
}

// Temperature Functions
float MAX30105_readTemperature(void) {
    MAX30105_writeRegister8(MAX30105_DIETEMPCONFIG, 0x01);
    
    uint32_t startTime = 0;
    while (startTime < 100) {
        uint8_t response = MAX30105_readRegister8(MAX30105_INTSTAT2);
        if ((response & MAX30105_INT_DIE_TEMP_RDY_ENABLE) > 0) break;
        CyDelay(1);
        startTime++;
    }
    
    int8_t tempInt = (int8_t)MAX30105_readRegister8(MAX30105_DIETEMPINT);
    uint8_t tempFrac = MAX30105_readRegister8(MAX30105_DIETEMPFRAC);
    
    return (float)tempInt + ((float)tempFrac * 0.0625f);
}

float MAX30105_readTemperatureF(void) {
    float temp = MAX30105_readTemperature();
    return temp * 1.8f + 32.0f;
}

void MAX30105_processSample(void) {
    // Check if there is at least one sample
    if (MAX30105_available() == 0) return;

    // Start I2C read of FIFO
    if (I2C_I2CMasterSendStart(MAX30105_I2C_ADDR, I2C_I2C_WRITE_XFER_MODE, MAX30105_TIMEOUT) != I2C_I2C_MSTR_NO_ERROR) return;
    if (I2C_I2CMasterWriteByte(MAX30105_FIFODATA, MAX30105_TIMEOUT) != I2C_I2C_MSTR_NO_ERROR) return;
    if (I2C_I2CMasterSendRestart(MAX30105_I2C_ADDR, I2C_I2C_READ_XFER_MODE, MAX30105_TIMEOUT) != I2C_I2C_MSTR_NO_ERROR) return;

    uint8_t b1, b2, b3;

    // RED LED
    b1 = i2c_read8(I2C_I2C_ACK_DATA);
    b2 = i2c_read8(I2C_I2C_ACK_DATA);
    b3 = i2c_read8(I2C_I2C_ACK_DATA);
    uint32_t redLong = ((uint32_t)b1 << 16) | ((uint32_t)b2 << 8) | b3;
    redLong &= 0x3FFFF;
    sense.head = (sense.head + 1) % STORAGE_SIZE;
    sense.red[sense.head] = redLong;

    // IR LED (if enabled)
    if (activeLEDs > 1) {
        b1 = i2c_read8(I2C_I2C_ACK_DATA);
        b2 = i2c_read8(I2C_I2C_ACK_DATA);
        b3 = i2c_read8(I2C_I2C_NAK_DATA);
        uint32_t irLong = ((uint32_t)b1 << 16) | ((uint32_t)b2 << 8) | b3;
        irLong &= 0x3FFFF;
        sense.IR[sense.head] = irLong;
    }

    // Finished reading this one sample
    MAX30105_nextSample();

    I2C_I2CMasterSendStop(MAX30105_TIMEOUT);
}


/* [] END OF FILE */
