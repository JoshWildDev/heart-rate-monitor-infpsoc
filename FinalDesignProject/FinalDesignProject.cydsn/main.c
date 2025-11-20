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

#define BLINK_DELAY 500

//volatile bool btn_pressed = false;
//volatile bool device_on = true;
char8 stringBuffer[100];

//CY_ISR_PROTO(Button_ISR_Handler);
//CY_ISR(Button_ISR_Handler){
//    if (!btn_pressed) {
//        btn_pressed = true;
//    }
//    PWR_BTN_ClearInterrupt();
// }

// static void handle_power_toggle(void){
//    if(device_on){
        //SSD1306_Clear();
        //SSD1306_Update();
//        I2C_Stop();
        
//        device_on = false;
        
//        CyDelay(5);
        
//        CySysPmDeepSleep();
//    } else {
//        I2C_Start();
        
        //OLED_RST_Write(0);
        //CyDelay(10);
        //OLED_RST_Write(1);
        //CyDelay(10);
        
        //SSD1306_Init();
        //SSD1306_Clear();
        //SSD1306_Update();
        
//        device_on = true;
//    }
//}

/*
int main(void)
{
    CyGlobalIntEnable;
 
    I2C_Start();
    UART_Start();
    
    UART_UartPutString("\n\rStarting Code\n\r");
    
    if (!init()) {
        UART_UartPutString("init failed!\n\r");
        for(;;) {
            LED_PWM_Start();
            CyDelay(1000);
            LED_PWM_Stop();
            CyDelay(1000);
        }
    }
    
    clearDisplay();
    drawString(0, 0, "Heart Rate: 75bpm", SSD1306_WHITE);
    drawString(0, 10, "Pulse Ox: 98%", SSD1306_WHITE);
    
    refreshDisplay();
    
    for(;;) {
        LED_PWM_Start();
    }
}
*/


/*
int main(void) {
     CyGlobalIntEnable;

    UART_Start();
    I2C_Start();

    UART_UartPutString("MAX30102 Test Program Starting...\r\n");

    // ------------------------------
    // Initialize MAX30102
    // ------------------------------
    if (!MAX30105_begin())
    {
        UART_UartPutString("ERROR: MAX30102 not detected!\r\n");
        for(;;);  // Stop here
    }
    
    UART_UartPutString("MAX30102 detected.\r\n");

    // Setup sensor: (powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange)
    // Red+IR, 100 samples/sec, normal defaults
    MAX30105_setup(
        0x3F,   // powerLevel (LED brightness)
        4,      // sampleAverage
        2,      // ledMode = RED + IR
        100,    // sampleRate
        411,    // pulseWidth
        16384   // adcRange
    );

    UART_UartPutString("MAX30102 configured. Starting data read...\r\n");

    char buffer[64];

    for (;;)
    {
        // This will pull new samples from the FIFO into sense.red/IR buffers
        //UART_UartPutString("Checking MAX30102...\r\n");
        MAX30105_check();

        while (MAX30105_available())
        {
            //UART_UartPutString("MAX30102 Available...\r\n");
            uint32_t red   = MAX30105_getFIFORed();
            uint32_t ir    = MAX30105_getFIFOIR();

            // Format and print to UART
            snprintf(buffer, sizeof(buffer),
                     "RED: %lu    IR: %lu\r\n",
                     (unsigned long)red,
                     (unsigned long)ir);

            UART_UartPutString(buffer);

            MAX30105_nextSample();
        }
        //UART_UartPutString("MAX30102 Exited...\r\n");

        CyDelay(1000);
    }  
}
*/

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
        0x3F,   // LED brightness
        4,      // sample average
        2,      // red + IR
        100,    // sampleRate
        411,    // pulseWidth
        16384   // adcRange
    );

    UART_UartPutString("MAX30102 configured.\r\n");

    clearDisplay();
    drawString(0, 0, "Sensor Ready", SSD1306_WHITE);
    refreshDisplay();

    char buffer[64];

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

            // ----------------------------------
            // UART PRINT (optional)
            // ----------------------------------
            snprintf(buffer, sizeof(buffer),
                    "RED: %lu   IR: %lu\r\n",
                    (unsigned long)red,
                    (unsigned long)ir);
            UART_UartPutString(buffer);

            // ----------------------------------
            // OLED PRINT
            // ----------------------------------
            char redStr[12];
            char irStr[12];

            // Convert numbers to strings
            snprintf(redStr, sizeof(redStr), "%lu", (unsigned long)red);
            snprintf(irStr, sizeof(irStr), "%lu", (unsigned long)ir);
            
            clearDisplay();

            drawString(0, 0, "MAX30102 Live Data", SSD1306_WHITE);

            drawString(0, 12, "RED:", SSD1306_WHITE);
            drawString(40, 12, redStr, SSD1306_WHITE);

            drawString(0, 24, "IR :", SSD1306_WHITE);
            drawString(40, 24, irStr, SSD1306_WHITE);

            refreshDisplay();

            MAX30105_nextSample();
        }

        CyDelay(500);  // Adjust refresh speed
    }
}


/* [] END OF FILE */
