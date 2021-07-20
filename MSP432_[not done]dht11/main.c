/***
 * Sample project
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DHT11_PORT  GPIO_PORT_P5
#define DHT11_PIN   GPIO_PIN1

static void print_uart(char *msg);
static void config_uart(void);
void Request(void);
void Response(void);
uint8_t Receive(void);

void Request(void)
{
    //define DHT11_PIN as output
    GPIO_setAsOutputPin(DHT11_PORT, DHT11_PIN);
    GPIO_setOutputLowOnPin(DHT11_PORT, DHT11_PIN);
    __delay_cycles(240000); //delay 20ms
    GPIO_setOutputHighOnPin(DHT11_PORT, DHT11_PIN);
    //__delay_cycles(480000); //delay 40ms
}

void Response(void)
{
    GPIO_setAsInputPin(DHT11_PORT, DHT11_PIN);
    while (GPIO_getInputPinValue(DHT11_PORT, DHT11_PIN))
        ;
    while (GPIO_getInputPinValue(DHT11_PORT, DHT11_PIN) == 0)
        ;
    while (GPIO_getInputPinValue(DHT11_PORT, DHT11_PIN))
        ;
}

uint8_t Receive(void)
{
    int i = 0;
    uint8_t c = 0;

    for (i = 0; i < 8; i++)
    {
        while (GPIO_getInputPinValue(DHT11_PORT, DHT11_PIN) == 0)
            ;
        __delay_cycles(360000); //delay 30ms
        if (GPIO_getInputPinValue(DHT11_PORT, DHT11_PIN))
        {
            c = (c << 1) | 0x01;
        }
        else
        {
            c = c << 1;
        }
        while (GPIO_getInputPinValue(DHT11_PORT, DHT11_PIN) == 1)
            ;
    }
    return c;
}

int main(void)
{
    uint8_t I_RH, D_RH, I_temp, D_temp, Checksum;
    char data[50];

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

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

    print_uart("Start\r\n");

    while (1)
    {
         Request();
         Response();
         //Read 40 bit data
         I_RH = Receive();
         D_RH = Receive();
         I_temp = Receive();
         D_temp = Receive();
         Checksum = Receive();
         //Check error
         if ((I_RH + D_RH + I_temp + D_temp) != Checksum)
         {
         print_uart("Error\r\n");
         }
         else
         {
         sprintf(data, "Humidity: %d.%d", D_RH, I_RH);
         print_uart(data);
         sprintf(data, "Temp: %d.%d", D_temp, I_temp);
         print_uart(data);
         }
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
