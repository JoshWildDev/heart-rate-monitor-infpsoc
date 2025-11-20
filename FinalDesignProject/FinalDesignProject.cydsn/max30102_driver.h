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

#ifndef MAX30105_H
#define MAX30105_H

#include <project.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// I2C Configuration
#define MAX30105_I2C_ADDR       0x57
#define MAX30105_TIMEOUT        100

// Register Addresses
#define MAX30105_INTSTAT1       0x00
#define MAX30105_INTSTAT2       0x01
#define MAX30105_INTENABLE1     0x02
#define MAX30105_INTENABLE2     0x03

// FIFO Registers
#define MAX30105_FIFOWRITEPTR   0x04
#define MAX30105_FIFOOVERFLOW   0x05
#define MAX30105_FIFOREADPTR    0x06
#define MAX30105_FIFODATA       0x07

// Configuration Registers
#define MAX30105_FIFOCONFIG     0x08
#define MAX30105_MODECONFIG     0x09
#define MAX30105_PARTICLECONFIG 0x0A
#define MAX30105_LED1_PULSEAMP  0x0C  // Red LED
#define MAX30105_LED2_PULSEAMP  0x0D  // IR LED
#define MAX30105_LED3_PULSEAMP  0x0E  // Green LED
#define MAX30105_MULTILEDCONFIG1 0x11
#define MAX30105_MULTILEDCONFIG2 0x12

// Die Temperature Registers
#define MAX30105_DIETEMPINT     0x1F
#define MAX30105_DIETEMPFRAC    0x20
#define MAX30105_DIETEMPCONFIG  0x21

// Part ID Registers
#define MAX30105_REVISIONID     0xFE
#define MAX30105_PARTID         0xFF

// Expected Part ID
#define MAX30105_EXPECTEDPARTID 0x15

// Configuration Values
// FIFO Configuration
#define MAX30105_SAMPLEAVG_MASK     0x1F
#define MAX30105_SAMPLEAVG_1        0x00
#define MAX30105_SAMPLEAVG_2        0x20
#define MAX30105_SAMPLEAVG_4        0x40
#define MAX30105_SAMPLEAVG_8        0x60
#define MAX30105_SAMPLEAVG_16       0x80
#define MAX30105_SAMPLEAVG_32       0xA0

#define MAX30105_ROLLOVER_MASK      0xEF
#define MAX30105_ROLLOVER_ENABLE    0x10
#define MAX30105_ROLLOVER_DISABLE   0x00

#define MAX30105_A_FULL_MASK        0xF0

// Mode Configuration
#define MAX30105_SHUTDOWN_MASK      0x7F
#define MAX30105_SHUTDOWN           0x80
#define MAX30105_WAKEUP             0x00

#define MAX30105_RESET_MASK         0xBF
#define MAX30105_RESET              0x40

#define MAX30105_MODE_MASK          0xF8
#define MAX30105_MODE_REDONLY       0x02
#define MAX30105_MODE_REDIRONLY     0x03
#define MAX30105_MODE_MULTILED      0x07

// Particle Sensing Configuration
#define MAX30105_ADCRANGE_MASK      0x9F
#define MAX30105_ADCRANGE_2048      0x00
#define MAX30105_ADCRANGE_4096      0x20
#define MAX30105_ADCRANGE_8192      0x40
#define MAX30105_ADCRANGE_16384     0x60

#define MAX30105_SAMPLERATE_MASK    0xE3
#define MAX30105_SAMPLERATE_50      0x00
#define MAX30105_SAMPLERATE_100     0x04
#define MAX30105_SAMPLERATE_200     0x08
#define MAX30105_SAMPLERATE_400     0x0C
#define MAX30105_SAMPLERATE_800     0x10
#define MAX30105_SAMPLERATE_1000    0x14
#define MAX30105_SAMPLERATE_1600    0x18
#define MAX30105_SAMPLERATE_3200    0x1C

#define MAX30105_PULSEWIDTH_MASK    0xFC
#define MAX30105_PULSEWIDTH_69      0x00
#define MAX30105_PULSEWIDTH_118     0x01
#define MAX30105_PULSEWIDTH_215     0x02
#define MAX30105_PULSEWIDTH_411     0x03

// Multi-LED Mode Configuration
#define SLOT_NONE                   0x00
#define SLOT_RED_LED                0x01
#define SLOT_IR_LED                 0x02
#define SLOT_GREEN_LED              0x03

#define MAX30105_SLOT1_MASK         0xF8
#define MAX30105_SLOT2_MASK         0x8F
#define MAX30105_SLOT3_MASK         0xF8
#define MAX30105_SLOT4_MASK         0x8F

// Interrupt Configuration
#define MAX30105_INT_DIE_TEMP_RDY_ENABLE 0x02

// Data Storage
#define STORAGE_SIZE 4  // Reduced for memory efficiency

typedef struct {
    uint32_t red[STORAGE_SIZE];
    uint32_t IR[STORAGE_SIZE];
    uint32_t green[STORAGE_SIZE];
    uint8_t head;
    uint8_t tail;
} sense_struct;

// Global variables
extern sense_struct sense;
extern uint8_t activeLEDs;
extern uint8_t revisionID;

// Function Prototypes
// Initialization
bool MAX30105_begin(void);
void MAX30105_setup(uint8_t powerLevel, uint8_t sampleAverage, uint8_t ledMode, 
                    int sampleRate, int pulseWidth, int adcRange);

// Configuration
void MAX30105_softReset(void);
void MAX30105_shutDown(void);
void MAX30105_wakeUp(void);
void MAX30105_setLEDMode(uint8_t mode);
void MAX30105_setADCRange(uint8_t adcRange);
void MAX30105_setSampleRate(uint8_t sampleRate);
void MAX30105_setPulseWidth(uint8_t pulseWidth);
void MAX30105_setPulseAmplitudeRed(uint8_t amplitude);
void MAX30105_setPulseAmplitudeIR(uint8_t amplitude);
void MAX30105_setPulseAmplitudeGreen(uint8_t amplitude);
void MAX30105_setFIFOAverage(uint8_t numberOfSamples);
void MAX30105_enableFIFORollover(void);
void MAX30105_clearFIFO(void);
void MAX30105_enableSlot(uint8_t slotNumber, uint8_t device);

// Data Collection
uint16_t MAX30105_check(void);
bool MAX30105_safeCheck(uint8_t maxTimeToCheck);
uint8_t MAX30105_available(void);
void MAX30105_nextSample(void);
uint32_t MAX30105_getRed(void);
uint32_t MAX30105_getIR(void);
uint32_t MAX30105_getGreen(void);
uint32_t MAX30105_getFIFORed(void);
uint32_t MAX30105_getFIFOIR(void);
uint32_t MAX30105_getFIFOGreen(void);

// Temperature
float MAX30105_readTemperature(void);
float MAX30105_readTemperatureF(void);

// Device ID
uint8_t MAX30105_readPartID(void);
void MAX30105_readRevisionID(void);
uint8_t MAX30105_getRevisionID(void);

// Low-level I2C
uint8_t MAX30105_readRegister8(uint8_t reg);
void MAX30105_writeRegister8(uint8_t reg, uint8_t value);
void MAX30105_bitMask(uint8_t reg, uint8_t mask, uint8_t thing);
uint8_t MAX30105_getWritePointer(void);
uint8_t MAX30105_getReadPointer(void);

#endif // MAX30105_H

/* [] END OF FILE */
