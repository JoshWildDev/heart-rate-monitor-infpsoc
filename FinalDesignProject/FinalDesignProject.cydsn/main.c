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
    drawString(0, 0, "HELLO WORLD", SSD1306_WHITE);
    drawString(0, 10, "12345", SSD1306_WHITE);
    
    refreshDisplay();
    
    for(;;) {
        LED_PWM_Start();
    }
}

/* [] END OF FILE */
