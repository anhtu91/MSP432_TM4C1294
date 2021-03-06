/***
 * Sample project
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "fatfs_sd.h"
#include "SysTick.h"
#include "UART_Transmit.h"

#define SD_CS_PORT          GPIO_PORT_P5
#define SD_CS_PIN           GPIO_PIN0

FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];  // to store strings..

char msg[50];

static volatile uint32_t mclk, smclk;

void delay_ms(uint32_t second_delay);
void delay1ms(void);
int bufsize(char *buf);
void clear_buffer(void);

const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 78, 2, 0,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
        EUSCI_A_UART_8_BIT_LEN };

const eUSCI_SPI_MasterConfig spiMasterConfig = {
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,                              // SMCLK Clock Source
        12000000,                                                   // SMCLK = DCO = 12MHZ
        2300000,                                                    // SPICLK = 2,3MHz
        EUSCI_SPI_MSB_FIRST,                                      // MSB First
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase CPHA = 1
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,                   // LOW polarity CPOL = 0
        EUSCI_B_SPI_3PIN                                            // 3Wire SPI Mode
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

    /* Configuring Timer32 to 12 (1us) of MCLK in periodic mode */
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_PERIODIC_MODE);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    mclk = CS_getMCLK();
    smclk = CS_getSMCLK();

    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);

    /* For usb logic analyzer */
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);

    //Enable CS PIN for SPI
    GPIO_setAsOutputPin(SD_CS_PORT, SD_CS_PIN);
    GPIO_setOutputLowOnPin(SD_CS_PORT, SD_CS_PIN);

    sprintf(msg, "MCLK = %d\r\n", mclk);
    print_uart(msg);
    sprintf(msg, "SMCLK = %d\r\n", smclk);
    print_uart(msg);

    SysTick_Init();
    MX_FATFS_Init();

    SysTick_Delay(500);

    fresult = f_mount(&fs, "/", 1);
    if (fresult != FR_OK)
        print_uart("ERROR!!! in mounting SD CARD...\r\n");
    else
        print_uart("SD CARD mounted successfully...\r\n");

    while (1)
    {

    }
}

void delay1ms(void)
{
    Timer32_setCount(TIMER32_BASE, 12000);
    Timer32_startTimer(TIMER32_BASE, true);
    while (Timer32_getValue(TIMER32_0_BASE) > 0)
    {

    }
}

void delay_ms(uint32_t second_delay)
{
    while (second_delay > 0)
    {
        delay1ms();
        second_delay--;
    }
}

int bufsize(char *buf)
{
    int i = 0;
    while (*buf++ != '\0')
        i++;
    return i;
}

void clear_buffer(void)
{
    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = '\0';
}
