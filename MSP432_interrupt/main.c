/***
 * Everytime P1.1 is pressed => interrupt => blink P1.0 and send number of press via uart to host machine
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void print_uart(uint32_t uart_module, char *msg);
static void config_uart(void);
static uint8_t button_pressed = 0;

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

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    //Set pin as input with pull up resistor
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    //Clear interrupt flag
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    //Enable interrupt
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    //Enable Interrupt
    Interrupt_enableInterrupt(INT_PORT1);

    //Enable master interrupt
    Interrupt_enableMaster();

    while (1)
    {
        //Using debug with semihosting
        //printf("Testing...");
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

static void print_uart(uint32_t uart_module, char *msg)
{
    uint32_t i;

    for (i = 0; i < strlen(msg); i++)
    {
        UART_transmitData(uart_module, msg[i]);
    }
}

void PORT1_IRQHandler(void)
{
    uint32_t status;
    char msg[50] = { 0 };

    //return which port has interrupt. In this case P1.1
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    //clear interrupt flag
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1)
    {
        while (GPIO_getInputPinValue(GPIO_PIN1, GPIO_PORT_P1))
            ;

        if (button_pressed % 2 == 0)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
        else
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }

        button_pressed++;
        sprintf(msg, "Press button %d\r\n", button_pressed);
        print_uart(EUSCI_A0_BASE, msg);
    }
}
