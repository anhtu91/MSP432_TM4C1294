/***
 * Sample project
 *
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "fatfs.h"

#define SD_CS_GPIO_Port     GPIO_PORT_P5 //GPIO_PORT_P4
#define SD_CS_Pin           GPIO_PIN2 //GPIO_PIN6

void spi_Start(void);
void delay1S(void);
void delay(uint32_t second_delay);
void print_uart(char *msg);
char msg[50];

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://processors.wiki.ti.com/index.php/
 *               USCI_UART_Baud_Rate_Gen_Mode_Selection
 */
const eUSCI_UART_ConfigV1 uartConfig = {
EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        26,                                      // BRDIV = 26
        0,                                       // UCxBRF = 0
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION  // Low Frequency Mode
        };

/* SPI Master Configuration Parameter */
const eUSCI_SPI_MasterConfig spiMasterConfig = {
EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        3000000,                                   // SMCLK 3Mhz
        500000,                                    // SPICLK = 500khz
        EUSCI_B_SPI_MSB_FIRST,                     // MSB First
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, // High polarity
        EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode
        };

//Delay 10ms
void SysTick_Handler(void)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

int main(void)
{

    /* Halting WDT and disabling master interrupts */
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    /* Initialize main clock to 3MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Start timer32 to create delay
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_PERIODIC_MODE);

    /* Selecting P1.2 and P1.3 in UART mode. */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
    GPIO_PIN2 | GPIO_PIN3,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Selecting P1.0 as output (LED). */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
    GPIO_PIN0,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configure SysTick for a 100Hz interrupt.  The FatFs driver wants a 10 ms
     * tick.
     */
    SysTick_setPeriod(3000000 / 100);
    SysTick_enableModule();
    SysTick_enableInterrupt();

    spi_Start();

    SPI_transmitData(EUSCI_B0_BASE, 0x19);
    while (SPI_isBusy(EUSCI_B0_BASE) == EUSCI_SPI_BUSY)
        ;
    uint8_t receive = SPI_receiveData(EUSCI_B0_BASE);
    sprintf(msg, "Receive: %d\r\n", receive);
    print_uart(msg);

    SPI_transmitData(EUSCI_B0_BASE, 0x86);
    while (SPI_isBusy(EUSCI_B0_BASE) == EUSCI_SPI_BUSY)
        ;
    receive = SPI_receiveData(EUSCI_B0_BASE);
    sprintf(msg, "Receive: %d\r\n", receive);
    print_uart(msg);

    /* Main while loop */
    while (1)
    {

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

void spi_Start(void)
{
    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
    GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* CS setup. */
    GPIO_setAsOutputPin(SD_CS_GPIO_Port, SD_CS_Pin);
    GPIO_setOutputLowOnPin(SD_CS_GPIO_Port, SD_CS_Pin);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);
}

void delay1S(void)
{
    Timer32_setCount(TIMER32_BASE, 3000000);
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
