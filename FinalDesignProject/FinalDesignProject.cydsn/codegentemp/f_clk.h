/*******************************************************************************
* File Name: f_clk.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_f_clk_H)
#define CY_CLOCK_f_clk_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void f_clk_StartEx(uint32 alignClkDiv);
#define f_clk_Start() \
    f_clk_StartEx(f_clk__PA_DIV_ID)

#else

void f_clk_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void f_clk_Stop(void);

void f_clk_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 f_clk_GetDividerRegister(void);
uint8  f_clk_GetFractionalDividerRegister(void);

#define f_clk_Enable()                         f_clk_Start()
#define f_clk_Disable()                        f_clk_Stop()
#define f_clk_SetDividerRegister(clkDivider, reset)  \
    f_clk_SetFractionalDividerRegister((clkDivider), 0u)
#define f_clk_SetDivider(clkDivider)           f_clk_SetDividerRegister((clkDivider), 1u)
#define f_clk_SetDividerValue(clkDivider)      f_clk_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define f_clk_DIV_ID     f_clk__DIV_ID

#define f_clk_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define f_clk_CTRL_REG   (*(reg32 *)f_clk__CTRL_REGISTER)
#define f_clk_DIV_REG    (*(reg32 *)f_clk__DIV_REGISTER)

#define f_clk_CMD_DIV_SHIFT          (0u)
#define f_clk_CMD_PA_DIV_SHIFT       (8u)
#define f_clk_CMD_DISABLE_SHIFT      (30u)
#define f_clk_CMD_ENABLE_SHIFT       (31u)

#define f_clk_CMD_DISABLE_MASK       ((uint32)((uint32)1u << f_clk_CMD_DISABLE_SHIFT))
#define f_clk_CMD_ENABLE_MASK        ((uint32)((uint32)1u << f_clk_CMD_ENABLE_SHIFT))

#define f_clk_DIV_FRAC_MASK  (0x000000F8u)
#define f_clk_DIV_FRAC_SHIFT (3u)
#define f_clk_DIV_INT_MASK   (0xFFFFFF00u)
#define f_clk_DIV_INT_SHIFT  (8u)

#else 

#define f_clk_DIV_REG        (*(reg32 *)f_clk__REGISTER)
#define f_clk_ENABLE_REG     f_clk_DIV_REG
#define f_clk_DIV_FRAC_MASK  f_clk__FRAC_MASK
#define f_clk_DIV_FRAC_SHIFT (16u)
#define f_clk_DIV_INT_MASK   f_clk__DIVIDER_MASK
#define f_clk_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_f_clk_H) */

/* [] END OF FILE */
