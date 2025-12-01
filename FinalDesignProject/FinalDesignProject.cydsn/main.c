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

#define BUFFER_LENGTH   100       // 2 seconds of data at 50Hz
#define IR_THRESHOLD    20000     // Threshold to detect finger
#define HISTORY_SIZE    5

// Buffers
uint32_t irBuffer[BUFFER_LENGTH];
uint32_t redBuffer[BUFFER_LENGTH];

// HR/SpO2 variables
int32_t heartRate = 0;
int8_t hrValid = 0;
int32_t spo2 = 0;
int8_t spo2Valid = 0;

// Display smoothing
int32_t hrHistory[HISTORY_SIZE] = {0};
int32_t spo2History[HISTORY_SIZE] = {0};
uint8_t historyIndex = 0;
uint8_t historyCount = 0;
int32_t displayHR = 0;
int32_t displaySpO2 = 0;

// Finger detection
bool fingerDetected = false;
uint8_t noFingerCount = 0;

// FIFO buffer index
uint8_t bufferIndex = 0;

// Timer ISR flag
volatile bool sampleReady = false;

// Forward declaration
void MAX30105_processSample(void);

// Timer ISR: called at 50Hz
CY_ISR(TimerISR_Handler)
{
    sampleReady = true;  // Set flag to read one sample in main loop
    Timer_1_ClearInterrupt(Timer_1_INTR_MASK_TC); // Clear interrupt
}

int main(void)
{
    CyGlobalIntEnable;

    // Initialize peripherals
    I2C_Start();
    UART_Start();
    UART_UartPutString("\n\rStarting MAX30102 + OLED Program...\n\r");

    if (!init()) {
        UART_UartPutString("OLED init failed!\n\r");
        for(;;) { LED_PWM_Start(); CyDelay(1000); LED_PWM_Stop(); CyDelay(1000); }
    }
    clearDisplay();

    if (!MAX30105_begin()) {
        UART_UartPutString("ERROR: MAX30102 not detected!\r\n");
        drawString(0, 0, "MAX30102 ERROR", SSD1306_WHITE);
        refreshDisplay();
        for(;;);
    }

    // MAX30102 setup
    MAX30105_setup(0x3F, 4, 2, 50, 411, 4096);
    drawString(0, 0, "Ready!", SSD1306_WHITE);
    drawString(0, 12, "Place finger", SSD1306_WHITE);
    refreshDisplay();
    CyDelay(1000);

    // Start Timer for 50Hz sampling
    Timer_1_Start();
    ISR_1_StartEx(TimerISR_Handler);

    char buffer[80];
    uint32_t calculationCount = 0;

    for(;;)
    {
        if (sampleReady)
        {
            sampleReady = false;

            // Read exactly one sample from sensor
            MAX30105_processSample();

            // Get latest sample
            uint32_t red = MAX30105_getFIFORed();
            uint32_t ir  = MAX30105_getFIFOIR();

            // Finger detection
            if (ir > IR_THRESHOLD) {
                fingerDetected = true;
                noFingerCount = 0;
            } else {
                noFingerCount++;
                if (noFingerCount > 10) fingerDetected = false;
            }

            // Store data in circular buffer
            redBuffer[bufferIndex] = red;
            irBuffer[bufferIndex]  = ir;
            bufferIndex++;

            if (bufferIndex >= BUFFER_LENGTH)
            {
                bufferIndex = 0;
                calculationCount++;

                if (fingerDetected)
                {
                    // Calculate HR and SpO2
                    maxim_heart_rate_and_oxygen_saturation(
                        irBuffer, BUFFER_LENGTH,
                        redBuffer,
                        &spo2, &spo2Valid,
                        &heartRate, &hrValid
                    );

                    // Debug UART
                    snprintf(buffer, sizeof(buffer),
                        "=== Calc #%lu ===\r\nHR: %ld (valid:%d), SpO2: %ld (valid:%d)\r\n",
                        (unsigned long)calculationCount,
                        (long)heartRate, hrValid,
                        (long)spo2, spo2Valid
                    );
                    if (hrValid || spo2Valid) {
                        UART_UartPutString(buffer);
                    }

                    // Heart Rate smoothing
                    if (hrValid && heartRate > 30 && heartRate < 180) {
                        hrHistory[historyIndex] = heartRate;
                        if (historyCount < HISTORY_SIZE) historyCount++;
                    } else if (historyCount > 0) {
                        uint8_t prevIdx = (historyIndex == 0) ? (HISTORY_SIZE-1) : (historyIndex-1);
                        hrHistory[historyIndex] = hrHistory[prevIdx];
                    }

                    // SpO2 smoothing with calibration
                    if (spo2Valid && spo2 > 70 && spo2 <= 100) {
                        int32_t correctedSpO2 = spo2 + 4;
                        if (correctedSpO2 > 100) correctedSpO2 = 100;
                        spo2History[historyIndex] = correctedSpO2;
                    } else if (historyCount > 0) {
                        uint8_t prevIdx = (historyIndex == 0) ? (HISTORY_SIZE-1) : (historyIndex-1);
                        spo2History[historyIndex] = spo2History[prevIdx];
                    }

                    historyIndex = (historyIndex + 1) % HISTORY_SIZE;

                    // Calculate display averages
                    int32_t hrSum = 0, spo2Sum = 0;
                    uint8_t hrCount = 0, spo2Count = 0;
                    for (uint8_t i = 0; i < historyCount; i++)
                    {
                        if (hrHistory[i] > 30 && hrHistory[i] < 180) { hrSum += hrHistory[i]; hrCount++; }
                        if (spo2History[i] > 70 && spo2History[i] <= 100) { spo2Sum += spo2History[i]; spo2Count++; }
                    }
                    displayHR = (hrCount > 0) ? hrSum / hrCount : 0;
                    displaySpO2 = (spo2Count > 0) ? spo2Sum / spo2Count : 0;
                }
            }
        }

        // -------------------- OLED DISPLAY --------------------
        uint32_t currentIR = MAX30105_getIR();
        clearDisplay();

        if (!fingerDetected || currentIR < IR_THRESHOLD)
        {
            drawString(0, 0, "Heart Monitor", SSD1306_WHITE);
            drawString(0, 15, "Please place finger", SSD1306_WHITE);
            drawString(0, 25, "on sensor", SSD1306_WHITE);
        }
        else
        {
            char hrStr[20], spo2Str[20];
            drawString(0, 0, "Heart Monitor", SSD1306_WHITE);

            // HR display
            if (displayHR > 30 && displayHR < 180)
                snprintf(hrStr, sizeof(hrStr), "HR: %ld bpm", (long)displayHR);
            else
                snprintf(hrStr, sizeof(hrStr), "HR: --");
            drawString(0, 12, hrStr, SSD1306_WHITE);

            // SpO2 display
            if (displaySpO2 > 70 && displaySpO2 <= 100)
                snprintf(spo2Str, sizeof(spo2Str), "SpO2: %ld%%", (long)displaySpO2);
            else
                snprintf(spo2Str, sizeof(spo2Str), "SpO2: --");
            drawString(0, 24, spo2Str, SSD1306_WHITE);

            // Signal quality
            if (currentIR > 80000)
                drawString(0, 40, "Signal: TOO HIGH!", SSD1306_WHITE);
            else if (currentIR > 50000)
                drawString(0, 40, "Signal: Good", SSD1306_WHITE);
            else if (currentIR > 20000)
                drawString(0, 40, "Signal: OK", SSD1306_WHITE);
            else
                drawString(0, 40, "Signal: Weak", SSD1306_WHITE);
        }

        refreshDisplay();
        CyDelay(10); // 100 Hz display update limit
    }
}


/* [] END OF FILE */
