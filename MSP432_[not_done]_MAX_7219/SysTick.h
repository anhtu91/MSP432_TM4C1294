#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>

#define SYSTICK_MAX_DELAY      0xFFFFFFFFU

void SysTick_Init(void);
void SysTick_Delay(uint32_t Delay);
uint32_t SysTick_GetTick(void);


