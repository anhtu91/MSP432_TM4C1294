/*
 * SysTick.c
 *
 *  Created on: 9 Mar 2021
 *      Author: ATN
 */
#include <SysTick.h>

volatile uint32_t uwTick;
volatile uint8_t FatFsCnt = 0;
volatile uint8_t Timer1, Timer2;

void SDTimer_Handler(void)
{
    if (Timer1 > 0)
        Timer1--;

    if (Timer2 > 0)
        Timer2--;
}

//Init SysTick with interrupt every 1ms
void SysTick_Init(void)
{
    uint32_t mclk = CS_getMCLK();
    SysTick_setPeriod(mclk / 1000);
    SysTick_enableModule();
    SysTick_enableInterrupt();

    Interrupt_enableMaster();
}

//Interrupt handler every 1ms
void SysTick_Handler(void)
{
    FatFsCnt++;
    if (FatFsCnt >= 10)
    {
        FatFsCnt = 0;
        SDTimer_Handler();
    }

    uwTick++;
    if (uwTick == SYSTICK_MAX_DELAY)
    {
        uwTick = 0;
    }
}

void SysTick_Delay(uint32_t Delay)
{
    uint32_t tickstart = SysTick_GetTick();
    uint32_t wait = Delay;

    while ((SysTick_GetTick() - tickstart) < wait)
    {
    }
}

uint32_t SysTick_GetTick(void)
{
    return uwTick;
}
