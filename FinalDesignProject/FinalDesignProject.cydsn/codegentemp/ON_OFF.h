/*******************************************************************************
* File Name: ON_OFF.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_ON_OFF_H)
#define CY_ISR_ON_OFF_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void ON_OFF_Start(void);
void ON_OFF_StartEx(cyisraddress address);
void ON_OFF_Stop(void);

CY_ISR_PROTO(ON_OFF_Interrupt);

void ON_OFF_SetVector(cyisraddress address);
cyisraddress ON_OFF_GetVector(void);

void ON_OFF_SetPriority(uint8 priority);
uint8 ON_OFF_GetPriority(void);

void ON_OFF_Enable(void);
uint8 ON_OFF_GetState(void);
void ON_OFF_Disable(void);

void ON_OFF_SetPending(void);
void ON_OFF_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the ON_OFF ISR. */
#define ON_OFF_INTC_VECTOR            ((reg32 *) ON_OFF__INTC_VECT)

/* Address of the ON_OFF ISR priority. */
#define ON_OFF_INTC_PRIOR             ((reg32 *) ON_OFF__INTC_PRIOR_REG)

/* Priority of the ON_OFF interrupt. */
#define ON_OFF_INTC_PRIOR_NUMBER      ON_OFF__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable ON_OFF interrupt. */
#define ON_OFF_INTC_SET_EN            ((reg32 *) ON_OFF__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the ON_OFF interrupt. */
#define ON_OFF_INTC_CLR_EN            ((reg32 *) ON_OFF__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the ON_OFF interrupt state to pending. */
#define ON_OFF_INTC_SET_PD            ((reg32 *) ON_OFF__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the ON_OFF interrupt. */
#define ON_OFF_INTC_CLR_PD            ((reg32 *) ON_OFF__INTC_CLR_PD_REG)



#endif /* CY_ISR_ON_OFF_H */


/* [] END OF FILE */
