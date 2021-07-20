/*
 * SysTick.c
 *
 *  Created on: 9 Mar 2021
 *      Author: ATN
 */
#include <SysTick.h>

volatile uint32_t uwTick;


//Init SysTick with interrupt every 10us
void SysTick_Init(void)
{
    uint32_t mclk = CS_getMCLK();
    SysTick_setPeriod(mclk / 100000);
    SysTick_enableModule();
    SysTick_enableInterrupt();

    Interrupt_enableMaster();
}

//Interrupt handler every 10us
void SysTick_Handler(void)
{
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
