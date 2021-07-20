/**
 * This example receive uart in interrupt mode then send back to transmitter
 *
 */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void print_uart(uint32_t uart_module, char *msg);
static void config_uart(void);

int main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 as output pin
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(
    GPIO_PORT_P1,
                                               GPIO_PIN2 | GPIO_PIN3,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    config_uart();

    //Enable UART Interrupt mode
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

    //Enable Interrupt
    Interrupt_enableInterrupt(INT_EUSCIA0);

    //make the CPU go to sleep after the SysTick interrupt
    Interrupt_enableSleepOnIsrExit();

    //Enable master interrupt
    Interrupt_enableMaster();

    while (1)
    {
        PCM_gotoLPM0();
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

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);
}

static void print_uart(uint32_t uart_module, char *msg)
{
    uint32_t i;

    for (i = 0; i < strlen(msg); i++)
    {
        UART_transmitData(uart_module, msg[i]);
    }
}

void EUSCIA0_IRQHandler(void)
{
    uint8_t receive_data = 0;
    uint32_t status = 0;
    char msg[50];

    status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    receive_data = UART_receiveData(EUSCI_A0_BASE);
    sprintf(msg, "\nEcho data: %c\r\n", receive_data);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        print_uart(EUSCI_A0_BASE, msg);
    }
}
