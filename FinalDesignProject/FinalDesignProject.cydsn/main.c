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

volatile bool btn_pressed = false;
volatile bool device_on = true;

CY_ISR_PROTO(Button_ISR_Handler);
CY_ISR(Button_ISR_Handler){
    if (!btn_pressed) {
        btn_pressed = true;
    }
    PWR_BTN_ClearInterrupt();
}

static void handle_power_toggle(void){
    if(device_on){
        //SSD1306_Clear();
        //SSD1306_Update();
        I2C_Stop();
        
        device_on = false;
        
        CyDelay(5);
        
        CySysPmDeepSleep();
    } else {
        I2C_Start();
        
        //OLED_RST_Write(0);
        //CyDelay(10);
        //OLED_RST_Write(1);
        //CyDelay(10);
        
        //SSD1306_Init();
        //SSD1306_Clear();
        //SSD1306_Update();
        
        device_on = true;
    }
}

int main(void)
{
    CyGlobalIntEnable;
 
    I2C_Start();
    //OLED_RST_Write(1);
 
    ON_OFF_Start();               
    ON_OFF_StartEx(Button_ISR_Handler);
 
    //SSD1306_Init();
    //SSD1306_Clear();
    //SSD1306_Update();
    LCD
 
    for (;;) {
        if (btn_pressed) {
            CyDelay(50);

            if (PWR_BTN_Read() == 0) {
                handle_power_toggle();
            }

            btn_pressed = false;

           PWR_BTN_ClearInterrupt();
        }
 
        if (device_on) {
            // normal operation code here
        } else {
            // After wake, code resumes here (or you can structure differently)
        }
 
        CySysPmSleep();
    }  
    
    
}

/* [] END OF FILE */
