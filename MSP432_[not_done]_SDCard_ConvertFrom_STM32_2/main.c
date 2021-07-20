#include <Hardware/SPI_Driver.h>
#include <Hardware/GPIO_Driver.h>
#include <Hardware/CS_Driver.h>
#include <Hardware/SysTick.h>
#include <Devices/MSPIO.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "fatfs_sd.h"
#include "fatfs.h"
#include "string.h"
#include "stdio.h"

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

/* SPI Configuration Parameter. These are the configuration parameters to
 * make the eUSCI B SPI module to operate with a 500KHz clock.*/
eUSCI_SPI_MasterConfig SPI0MasterConfig = {
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK, 3000000, 500000, //try 2,3MHz SPI
        EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT, //EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW, //high
        EUSCI_B_SPI_3PIN };

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

int bufsize(char *buf)
{
    int i = 0;
    while (*buf++ != '\0')
        i++;
    return i;
}

void clear_buffer(void)
{
    int i = 0;
    for (i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = '\0';
}

void main(void)
{
    WDT_A_holdTimer();

    //Clock source init
    CS_Init();

    /*Initialize all hardware required for the SD Card*/
    SPI_Init(EUSCI_B0_BASE, SPI0MasterConfig);
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

    SysTick_Delay(500);

    fresult = f_mount(&fs, "/", 1);
    if (fresult != FR_OK)
        MSPrintf(EUSCI_A0_BASE,"ERROR!!! in mounting SD CARD...\r\n");
    else
        MSPrintf(EUSCI_A0_BASE,"SD CARD mounted successfully...\r\n");

    /*************** Card capacity details ********************/

    /* Check free space */
    f_getfree("", &fre_clust, &pfs);

    total = (uint32_t) ((pfs->n_fatent - 2) * pfs->csize * 0.5);
    sprintf(buffer, "SD CARD Total Size: \t%lu\r\n", total);
    MSPrintf(EUSCI_A0_BASE, buffer);
    clear_buffer();
    free_space = (uint32_t) (fre_clust * pfs->csize * 0.5);
    sprintf(buffer, "SD CARD Free Space: \t%lu\r\n", free_space);
    MSPrintf(EUSCI_A0_BASE,buffer);
    clear_buffer();

    /************* The following operation is using PUTS and GETS *********************/

    /* Open file to write/ create a file if it doesn't exist */
    fresult = f_open(&fil, "file1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

    /* Writing text */
    f_puts("This NEW data is from the FILE1.txt. And it was written using ...f_puts... ",
           &fil);

    /* Close file */
    fresult = f_close(&fil);

    if (fresult == FR_OK)
        MSPrintf(EUSCI_A0_BASE,"File1.txt created and the data is written\r\n");

    /* Open file to read */
    fresult = f_open(&fil, "file1.txt", FA_READ);

    /* Read string from the file */
    f_gets(buffer, f_size(&fil), &fil);

    MSPrintf(EUSCI_A0_BASE,
            "File1.txt is opened and it contains the data as shown below\r\n");
    MSPrintf(EUSCI_A0_BASE,buffer);
    MSPrintf(EUSCI_A0_BASE,"\r\n");

    /* Close file */
    f_close(&fil);

    clear_buffer();

    /**************** The following operation is using f_write and f_read **************************/

    /* Create second file with read write access and open it */
    fresult = f_open(&fil, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE);

    /* Writing text */
    strcpy(buffer,
           "This is File2.txt, written using ...f_write... and it says Hello from Controllerstech\n");

    fresult = f_write(&fil, buffer, bufsize(buffer), &bw);

    MSPrintf(EUSCI_A0_BASE,"File2.txt created and data is written\r\n");

    /* Close file */
    f_close(&fil);

    // clearing buffer to show that result obtained is from the file
    clear_buffer();

    /* Open second file to read */
    fresult = f_open(&fil, "file2.txt", FA_READ);
    if (fresult == FR_OK)
        MSPrintf(EUSCI_A0_BASE,"file2.txt is open and the data is shown below\r\n");

    /* Read data from the file
     * Please see the function details for the arguments */
    f_read(&fil, buffer, f_size(&fil), &br);
    MSPrintf(EUSCI_A0_BASE,buffer);
    MSPrintf(EUSCI_A0_BASE,"\r\n");

    /* Close file */
    f_close(&fil);

    clear_buffer();

    /*********************UPDATING an existing file ***************************/

    /* Open the file with write access */
    fresult = f_open(&fil, "file2.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);

    /* Move to offset to the end of the file */
    fresult = f_lseek(&fil, f_size(&fil));

    if (fresult == FR_OK)
        MSPrintf(EUSCI_A0_BASE,"About to update the file2.txt\r\n");

    /* write the string to the file */
    fresult = f_puts("This is updated data and it should be in the end", &fil);

    f_close(&fil);

    clear_buffer();

    /* Open to read the file */
    fresult = f_open(&fil, "file2.txt", FA_READ);

    /* Read string from the file */
    fresult = f_read(&fil, buffer, f_size(&fil), &br);
    if (fresult == FR_OK)
        MSPrintf(EUSCI_A0_BASE,"Below is the data from updated file2.txt\r\n");
    MSPrintf(EUSCI_A0_BASE,buffer);
    MSPrintf(EUSCI_A0_BASE,"\r\n");

    /* Close file */
    f_close(&fil);

    clear_buffer();

    while (1)
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_P3, GPIO_PIN7);
        SysTick_Delay(300);
    }
}

