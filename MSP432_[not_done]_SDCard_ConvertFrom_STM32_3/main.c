#include <Hardware/SPI_Driver.h>
#include <Hardware/GPIO_Driver.h>
#include <Hardware/CS_Driver.h>
#include <Hardware/SysTick.h>
#include <Devices/MSPIO.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ff.h"

#define SD_CS_PORT      (GPIO_PORT_P5)
#define SD_CS_PIN       (GPIO_PIN0)

/* MISO = P1.7
 * MOSI = P1.6
 * SCK = P1.5
 * CS = P5.0
 * VCC = 3.3V
 * GND = GND
 */

/*Huge thanks to bluehash @ https://github.com/bluehash/MSP432Launchpad/tree/master/MSP432-Launchpad-FatFS-SDCard*/

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_ConfigV1 UART0Config = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 13, 0, 37,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION };

FATFS FatFs; /* FatFs work area needed for each volume */
FIL Fil; /* File object needed for each open file */

void main(void)
{
    WDT_A_holdTimer();

    //Clock source init
    CS_Init();

    /*Initialize all hardware required for the SD Card*/
    UART_Init(EUSCI_A0_BASE, UART0Config);
    GPIO_Init(SD_CS_PORT, SD_CS_PIN); //Start CS PIN

    //GPIO Output Init
    GPIO_Init(GPIO_PORT_P3, GPIO_PIN6); //Start extern LED
    GPIO_Init(GPIO_PORT_P3, GPIO_PIN7); //Start extern LED
    GPIO_Low(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_Low(GPIO_PORT_P3, GPIO_PIN7);

    SysTick_Init();

    char msg[50];
    uint32_t dco = CS_getDCOFrequency();
    sprintf(msg, "DCO Clock = %d\r\n", dco);
    MSPrintf(EUSCI_A0_BASE, msg);

    UINT bw;
    FRESULT fr;

    f_mount(&FatFs, "", 0); /* Give a work area to the default drive */

    fr = f_open(&Fil, "Hello.txt", FA_WRITE | FA_CREATE_ALWAYS); /* Create a file */
    if (fr == FR_OK)
    {
        f_write(&Fil, "Hello World", 11, &bw); /* Write data to the file */
        fr = f_close(&Fil); /* Close the file */
        if (fr == FR_OK && bw == 11)
        { /* Lights green LED if data written well */
            while (1)
            {
                GPIO_High(GPIO_PORT_P3, GPIO_PIN6);
                SysTick_Delay(1000);
                GPIO_Low(GPIO_PORT_P3, GPIO_PIN6);
                SysTick_Delay(1000);
            }
        }
    }

    while (1)
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_P3, GPIO_PIN7);
        SysTick_Delay(1);
    }
}

