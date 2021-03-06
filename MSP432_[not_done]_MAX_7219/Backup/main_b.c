/***
 * Sample project
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "SysTick.h"
#include "MAX7219.h"

#define SPI_PORT        (GPIO_PORT_P1)
#define SPI_MODULE      (EUSCI_B0_BASE)

#define SPI_CS_PORT     (GPIO_PORT_P3)
#define SPI_CS_PIN      (GPIO_PIN7)

#define SPI_MOSI_PORT   (GPIO_PORT_P1)
#define SPI_MOSI_PIN    (GPIO_PIN6)

#define SPI_CLK_PORT    (GPIO_PORT_P1)
#define SPI_CLK_PIN     (GPIO_PIN5)

void print_uart(char *msg);

const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 78, 2, 0,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
        EUSCI_A_UART_8_BIT_LEN };

const eUSCI_SPI_MasterConfig spiMasterConfig = {
EUSCI_B_SPI_CLOCKSOURCE_SMCLK,                             // SMCLK Clock Source
        3000000,                                         // SMCLK = DCO = 12MHZ
        22000,                                              // SPICLK = 2,3MHz
        EUSCI_B_SPI_MSB_FIRST,                                      // MSB First
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT, // Phase CPHA = 1
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,       // LOW polarity CPOL = 0
        EUSCI_B_SPI_3PIN                                      // 3Wire SPI Mode
        };

int main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Set P1.0 as output pin for LED
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(
    GPIO_PORT_P1,
                                               GPIO_PIN2 | GPIO_PIN3,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);

    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(
    SPI_PORT,
                                               SPI_CLK_PIN | SPI_MOSI_PIN,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 4wire master mode */
    SPI_initMaster(SPI_MODULE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(SPI_MODULE);

    SysTick_Init();

    //Enable CS PIN for SPI
    GPIO_setAsOutputPin(SPI_CS_PORT, SPI_CS_PIN);

    MAX7219_init(4);

    while (1)
    {
        MAX7219_displayText("ABCD");
    }
}

void print_uart(char *msg)
{
    uint32_t i;

    for (i = 0; i < strlen(msg); i++)
    {
        UART_transmitData(EUSCI_A0_BASE, msg[i]);
    }
}
