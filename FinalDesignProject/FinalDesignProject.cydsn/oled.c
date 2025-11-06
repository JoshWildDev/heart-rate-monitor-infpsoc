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

#define OLED_ADDR 0x3C
#define OLED_CMD 0x00
#define OLED_DATA 0x40

void OLED_Write_Byte(uint8_t control, uint8_t byte) {
    uint8_t buffer[2] = {control, byte};
    
   
}


/* [] END OF FILE */
