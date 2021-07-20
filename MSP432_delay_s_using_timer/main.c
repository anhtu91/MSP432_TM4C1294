/**
 *
 *  Create delay using timer
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void print_uart(char *msg);
void config_uart(void);
void delay1S(void);
void delay(uint32_t second_delay);

int main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    /* Setting MCLK to REFO at 128Khz for LF mode */
    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Configuring Timer32 to 128000 (1s) of MCLK in periodic mode */
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_PERIODIC_MODE);

    /* Configuring GPIO */
    /* For LED P1.0 on board */
    //MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    //MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* For usb logic analyzer */
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN2 | GPIO_PIN3,
            GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    config_uart();

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    print_uart("Delay using timer\r\n");

    while (1)
    {
        //MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_toggleOutputOnPin(GPIO_PORT_P3, GPIO_PIN7);
        delay(3);
    }
}

static void config_uart(void)
{
    eUSCI_UART_ConfigV1 uartConfig;
    uartConfig.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    uartConfig.clockPrescalar = 78;
    uartConfig.firstModReg = 2;
    uartConfig.secondModReg = 0;
    uartConfig.parity = EUSCI_A_UART_NO_PARITY;
    uartConfig.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    uartConfig.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    uartConfig.uartMode = EUSCI_A_UART_MODE;
    uartConfig.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
    uartConfig.dataLength = EUSCI_A_UART_8_BIT_LEN;

    //![Simple UART Example]
    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);
}

void print_uart(char *msg)
{
    uint32_t i;

    for (i = 0; i < strlen(msg); i++)
    {
        UART_transmitData(EUSCI_A0_BASE, msg[i]);
    }
}

void delay1S(void)
{
    Timer32_setCount(TIMER32_BASE, 128000);
    Timer32_startTimer(TIMER32_BASE, true);
    while (Timer32_getValue(TIMER32_0_BASE) > 0)
    {

    }
}

void delay(uint32_t second_delay)
{
    while (second_delay > 0)
    {
        delay1S();
        second_delay--;
    }
}
