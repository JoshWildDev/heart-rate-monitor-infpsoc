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


#include "heartrate.h"
#include "project.h"

int16_t IR_AC_Max = 20;
int16_t IR_AC_Min = -20;

int16_t IR_AC_Signal_Current = 0;
int16_t IR_AC_Signal_Previous;
int16_t IR_AC_Signal_min = 0;
int16_t IR_AC_Signal_max = 0;
int16_t IR_Average_Estimated;

int16_t positiveEdge = 0;
int16_t negativeEdge = 0;

int32_t ir_avg_reg = 0;

// circular buffer
int16_t cbuf[32];
uint8_t offset = 0;

static const uint16_t FIRCoeffs[12] =
{172, 321, 579, 927, 1360, 1858, 2390, 2916, 3391, 3768, 4012, 4096};

//-------------------------------------
// MAIN BEAT DETECTOR
//-------------------------------------
bool checkForBeat(int32_t sample)
{
    bool beatDetected = false;

    // Save previous AC value
    IR_AC_Signal_Previous = IR_AC_Signal_Current;

    // DC estimation (fixed for MAX30102)
    IR_Average_Estimated = averageDCEstimator(&ir_avg_reg, sample);

    // AC component
    IR_AC_Signal_Current = lowPassFIRFilter(sample - IR_Average_Estimated);

    // Rising zero-crossing
    if ((IR_AC_Signal_Previous < 0) && (IR_AC_Signal_Current >= 0))
    {
        IR_AC_Max = IR_AC_Signal_max;
        IR_AC_Min = IR_AC_Signal_min;

        positiveEdge = 1;
        negativeEdge = 0;

        IR_AC_Signal_max = 0;

        // amplitude threshold tuned for MAX30102
        int16_t amplitude = IR_AC_Max - IR_AC_Min;
        if (amplitude > 50 && amplitude < 4000)
        {
            beatDetected = true;
        }
    }

    // Falling zero-crossing
    if ((IR_AC_Signal_Previous > 0) && (IR_AC_Signal_Current <= 0))
    {
        positiveEdge = 0;
        negativeEdge = 1;
        IR_AC_Signal_min = 0;
    }

    // Track max
    if (positiveEdge && (IR_AC_Signal_Current > IR_AC_Signal_Previous))
    {
        IR_AC_Signal_max = IR_AC_Signal_Current;
    }

    // Track min
    if (negativeEdge && (IR_AC_Signal_Current < IR_AC_Signal_Previous))
    {
        IR_AC_Signal_min = IR_AC_Signal_Current;
    }

    return beatDetected;
}

//-------------------------------------
// DC REMOVE WITH SAFE RANGE
//-------------------------------------
int16_t averageDCEstimator(int32_t *p, int32_t x)
{
    // prevent overflow, slower response
    *p += (((x << 15) - *p) >> 5);
    return (*p >> 15);
}

//-------------------------------------
// FIR LOW PASS
//-------------------------------------
int16_t lowPassFIRFilter(int16_t din)
{
    cbuf[offset] = din;

    int32_t z = mul16(FIRCoeffs[11], cbuf[(offset - 11) & 0x1F]);

    for (uint8_t i = 0; i < 11; i++)
    {
        z += mul16(FIRCoeffs[i],
                   cbuf[(offset - i) & 0x1F] +
                   cbuf[(offset - 22 + i) & 0x1F]);
    }

    offset = (offset + 1) & 0x1F;

    return (z >> 15);
}

int32_t mul16(int16_t x, int16_t y)
{
    return ((int32_t)x * (int32_t)y);
}


/* [] END OF FILE */
