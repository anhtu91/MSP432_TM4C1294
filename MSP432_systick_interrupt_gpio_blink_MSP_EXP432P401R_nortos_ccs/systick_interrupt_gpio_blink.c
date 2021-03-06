/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 SysTick - Blink LED Program
 *
 * Description: This program will use the SysTick module to blink an LED with
 * a one second period. Once setup, the application will go into LPM3
 * mode and only wake up to toggle the GPIO pin.
 *
 * This program runs infinitely until manually halted by the user.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> P1.0 LED
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
  *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

//![Simple SysTick Example]
int main(void)
{
    /* Halting the Watchdog */
    WDT_A_holdTimer();

    /* Configuring GPIO as an output */
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);

    //CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    /* Configuring SysTick to trigger at 1500000 (MCLK is 3MHz so this will
     * make it toggle every 0,5s) Default MCLK is clock source of Systick and MCLK clock come from DCO*/
    SysTick_enableModule();
    SysTick_setPeriod(1500000);
    Interrupt_enableSleepOnIsrExit();
    SysTick_enableInterrupt();
    
    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();

    while (1)
    {
        PCM_gotoLPM0();
    }
}

void SysTick_Handler(void)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P3, GPIO_PIN6);
}
//![Simple SysTick Example]

