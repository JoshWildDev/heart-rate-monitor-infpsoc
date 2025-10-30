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

//=== Define Global Constants ===


int main(void)
{
    //=== Define Constants and Variables ===
    
    //enable global interrupts
    CyGlobalIntEnable;
    
    //=== System Initialization ===
    I2C_Start();                 //start I2C component
    UART_Start();                //start UART for debugging
    
    CyDelay(2000); // small startup delay
}

/* [] END OF FILE */
