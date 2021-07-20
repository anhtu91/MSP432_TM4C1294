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

static void print_uart(char *msg);
static void config_uart(void);

int main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);                   // 12000000 Hz for timer32 and uart
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // 12000000 Hz

    /* Check MCLK value */
    uint32_t mlck = CS_getMCLK();

    /* Configuring GPIO */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN2 | GPIO_PIN3,
            GPIO_PRIMARY_MODULE_FUNCTION);

    config_uart();

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    print_uart("Delay ms using timer\r\n");

    while (1)
    {
        /*
         * With CS_MCLK = 12 Mhz
         * 600 ~ 53us
         * 1200 ~ 0,1ms = 100us
         */
        __delay_cycles(1200);
        //delay_ms(2);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN5);
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

static void print_uart(char *msg)
{
    uint32_t i;

    for (i = 0; i < strlen(msg); i++)
    {
        UART_transmitData(EUSCI_A0_BASE, msg[i]);
    }
}
