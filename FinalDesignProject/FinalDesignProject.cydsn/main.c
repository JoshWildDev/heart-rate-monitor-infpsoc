/*===================================    
Final Design Project: Heart Rate Monitor  
Paige Spencer & Josh Wild    
October 30 2025    
Weeks 1-4:
Write code to successfully measure, filter, and display heart rate.
- Read IR sensor data from MAX30102
- Filter noise (moving average)
- Detect peaks to calculate BPM
- Display BPM on OLED
- Indicate heart rate range with messages:
    * Normal Resting (60–100 BPM)
    * Moderate-Intensity Exercise (100–140 BPM)
    * High Intensity Exercise (>140 BPM)
========================================*/   

#include "project.h"
#include "stdbool.h"
#include "stdio.h"
#include "ssd1306_driver.h"
#include "max30102_driver.h"
#include "heartrate.h"
#include "spo2_algorithm.h"

#define BLINK_DELAY 500
#define BUFFER_LENGTH 100  // Buffer for SpO2 calculation (4 seconds at 25Hz)

char8 stringBuffer[100];

// Buffers for SpO2 calculation
uint32_t irBuffer[BUFFER_LENGTH];
uint32_t redBuffer[BUFFER_LENGTH];

// Heart rate variables
int32_t heartRate = 0;
int8_t hrValid = 0;

// SpO2 variables
int32_t spo2 = 0;
int8_t spo2Valid = 0;

// Smoothing variables for display stability
#define HISTORY_SIZE 3  // Reduced for faster response
int32_t hrHistory[HISTORY_SIZE] = {0};
int32_t spo2History[HISTORY_SIZE] = {0};
uint8_t historyIndex = 0;
uint8_t historyCount = 0;

int main(void)
{
    CyGlobalIntEnable;
    I2C_Start();
    UART_Start();
    UART_UartPutString("\n\rStarting MAX30102 + OLED Program...\n\r");
    
    // ----------------------------------
    // OLED INITIALIZATION
    // ----------------------------------
    if (!init()) {
        UART_UartPutString("OLED init failed!\n\r");
        for(;;) {
            LED_PWM_Start();
            CyDelay(1000);
            LED_PWM_Stop();
            CyDelay(1000);
        }
    }
    
    clearDisplay();
    drawString(0, 0, "MAX30102 Init...", SSD1306_WHITE);
    refreshDisplay();
    
    // ----------------------------------
    // MAX30102 INITIALIZATION
    // ----------------------------------
    if (!MAX30105_begin())
    {
        UART_UartPutString("ERROR: MAX30102 not detected!\r\n");
        clearDisplay();
        drawString(0, 0, "MAX30102 ERROR", SSD1306_WHITE);
        refreshDisplay();
        for(;;);
    }
    
    UART_UartPutString("MAX30102 detected.\r\n");
    
    MAX30105_setup(
        0x1F,   // LED brightness
        4,      // sample average
        2,      // red + IR for SpO2
        25,     // sampleRate (Hz) - 25Hz to save memory
        411,    // pulseWidth (us)
        4096    // adcRange
    );
    
    UART_UartPutString("MAX30102 configured.\r\n");
    
    clearDisplay();
    drawString(0, 0, "Place finger on", SSD1306_WHITE);
    drawString(0, 12, "sensor...", SSD1306_WHITE);
    refreshDisplay();
    CyDelay(2000);
    
    char buffer[64];
    uint8_t bufferIndex = 0;
    bool dataReady = false;
    bool initialCalibration = true;  // Flag for initial data collection
    
    // ----------------------------------
    // MAIN LOOP
    // ----------------------------------
    for (;;)
    {
        MAX30105_check(); // Fill FIFO buffer
        
        while (MAX30105_available())
        {
            uint32_t red = MAX30105_getFIFORed();
            uint32_t ir  = MAX30105_getFIFOIR();
            
            // Store data in buffer for SpO2 calculation
            redBuffer[bufferIndex] = red;
            irBuffer[bufferIndex] = ir;
            bufferIndex++;
            
            // Display collection progress every 10 samples (only during initial calibration)
            if (initialCalibration && bufferIndex % 10 == 0)
            {
                clearDisplay();
                drawString(0, 0, "Collecting data", SSD1306_WHITE);
                snprintf(buffer, sizeof(buffer), "%d%%", bufferIndex);
                drawString(0, 12, buffer, SSD1306_WHITE);
                refreshDisplay();
            }
            
            // Once we have enough samples, calculate SpO2 and HR
            if (bufferIndex >= BUFFER_LENGTH)
            {
                // Calculate heart rate and SpO2
                maxim_heart_rate_and_oxygen_saturation(
                    irBuffer, 
                    BUFFER_LENGTH, 
                    redBuffer, 
                    &spo2, 
                    &spo2Valid, 
                    &heartRate, 
                    &hrValid
                );
                
                // Print to UART for debugging
                snprintf(buffer, sizeof(buffer),
                        "Raw - HR: %ld (valid: %d), SpO2: %ld (valid: %d)\r\n",
                        (long)heartRate, hrValid, (long)spo2, spo2Valid);
                UART_UartPutString(buffer);
                
                // Add to history for smoothing (only if valid and reasonable)
                if (hrValid && heartRate > 40 && heartRate < 180)
                {
                    hrHistory[historyIndex] = heartRate;
                    if (historyCount < HISTORY_SIZE) historyCount++;
                }
                else
                {
                    // If invalid, copy previous valid value to maintain stability
                    if (historyCount > 0)
                    {
                        uint8_t prevIndex = (historyIndex == 0) ? (HISTORY_SIZE - 1) : (historyIndex - 1);
                        hrHistory[historyIndex] = hrHistory[prevIndex];
                    }
                }
                
                if (spo2Valid && spo2 > 85 && spo2 <= 100)
                {
                    spo2History[historyIndex] = spo2;
                    if (historyCount < HISTORY_SIZE) historyCount++;
                }
                else
                {
                    // If invalid, copy previous valid value to maintain stability
                    if (historyCount > 0)
                    {
                        uint8_t prevIndex = (historyIndex == 0) ? (HISTORY_SIZE - 1) : (historyIndex - 1);
                        spo2History[historyIndex] = spo2History[prevIndex];
                    }
                }
                
                historyIndex = (historyIndex + 1) % HISTORY_SIZE;
                
                // Reset buffer index and mark data as ready
                bufferIndex = 0;
                dataReady = true;
                initialCalibration = false;  // No more "collecting" screens after first time
            }
            
            
            MAX30105_nextSample();
        }
        
        // ----------------------------------
        // OLED DISPLAY (runs independently of data collection)
        // ----------------------------------
        if (dataReady)
        {
            char hrStr[20];
            char spo2Str[20];
            
            // Calculate smoothed averages
            int32_t hrSmoothed = 0;
            int32_t spo2Smoothed = 0;
            uint8_t validHrCount = 0;
            uint8_t validSpo2Count = 0;
            
            for (uint8_t i = 0; i < historyCount; i++)
            {
                if (hrHistory[i] > 40 && hrHistory[i] < 180)
                {
                    hrSmoothed += hrHistory[i];
                    validHrCount++;
                }
                if (spo2History[i] > 85 && spo2History[i] <= 100)
                {
                    spo2Smoothed += spo2History[i];
                    validSpo2Count++;
                }
            }
            
            if (validHrCount > 0)
                hrSmoothed /= validHrCount;
            else
                hrSmoothed = 0;
                
            if (validSpo2Count > 0)
                spo2Smoothed /= validSpo2Count;
            else
                spo2Smoothed = 0;
            
            // Get latest IR value for signal quality
            uint32_t currentIR = MAX30105_getIR();
            
            clearDisplay();
            drawString(0, 0, "Heart Monitor", SSD1306_WHITE);
            
            // Display Heart Rate (smoothed or current)
            if (validHrCount > 0 && hrSmoothed > 40 && hrSmoothed < 180)
            {
                snprintf(hrStr, sizeof(hrStr), "HR: %ld bpm", (long)hrSmoothed);
            }
            else if (hrValid && heartRate > 40 && heartRate < 180)
            {
                // Show current reading if no history yet
                snprintf(hrStr, sizeof(hrStr), "HR: %ld bpm*", (long)heartRate);
            }
            else
            {
                snprintf(hrStr, sizeof(hrStr), "HR: --");
            }
            drawString(0, 12, hrStr, SSD1306_WHITE);
            
            // Display SpO2 (smoothed or current)
            if (validSpo2Count > 0 && spo2Smoothed > 85 && spo2Smoothed <= 100)
            {
                snprintf(spo2Str, sizeof(spo2Str), "SpO2: %ld%%", (long)spo2Smoothed);
            }
            else if (spo2Valid && spo2 > 85 && spo2 <= 100)
            {
                // Show current reading if no history yet
                snprintf(spo2Str, sizeof(spo2Str), "SpO2: %ld%%*", (long)spo2);
            }
            else
            {
                snprintf(spo2Str, sizeof(spo2Str), "SpO2: --");
            }
            drawString(0, 24, spo2Str, SSD1306_WHITE);
            
            // Show signal quality indicator
            if (currentIR > 50000)
            {
                drawString(0, 36, "Signal: Good", SSD1306_WHITE);
            }
            else if (currentIR > 20000)
            {
                drawString(0, 36, "Signal: OK", SSD1306_WHITE);
            }
            else
            {
                drawString(0, 36, "Place finger!", SSD1306_WHITE);
            }
            
            // Show data stability indicator
            if (historyCount >= HISTORY_SIZE)
            {
                drawString(0, 48, "Readings stable", SSD1306_WHITE);
            }
            else if (historyCount > 0)
            {
                snprintf(buffer, sizeof(buffer), "Stabilizing %d/3", historyCount);
                drawString(0, 48, buffer, SSD1306_WHITE);
            }
            else
            {
                drawString(0, 48, "Analyzing...", SSD1306_WHITE);
            }
            
            refreshDisplay();
        }
        
        CyDelay(50);  // Reduced delay for more responsive display updates
    }
}

/* [] END OF FILE */
