/**
 *
 * Read C02 sensor CSS811 via I2C
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <lib/ccs811.h>

#define CCS811_SLAVEADDR_0     0x5A
#define CCS811_SLAVEADDR_1     0x5B

#define EUSCI_I2C_MODULE       EUSCI_B1_BASE

void print_uart(char *msg);
void delay_us(uint32_t num);

const eUSCI_I2C_MasterConfig i2cConfig = {
EUSCI_B_I2C_CLOCKSOURCE_SMCLK,                  // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
        };

const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 78, 2, 0,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
        EUSCI_A_UART_8_BIT_LEN };

int main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(
    GPIO_PORT_P1,
                                               GPIO_PIN2 | GPIO_PIN3,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Set function i2c for port */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
    GPIO_PIN4 | GPIO_PIN5,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    I2C_initMaster(EUSCI_I2C_MODULE, &i2cConfig);
    I2C_setSlaveAddress(EUSCI_I2C_MODULE, CCS811_SLAVEADDR_0);
    I2C_enableModule(EUSCI_I2C_MODULE);

    print_uart("Start...\r\n");
    if (ccs811_begin())
    {
        print_uart("OK\r\n");
    }
    else
    {
        print_uart("Failed\r\n");
    }

    uint8_t buf = 0x00;
    uint8_t hw_id;
    bool ok;
    char msg[50];
    //sprintf(msg, "sprintf\r\n");
    //print_uart(msg);

    ok = ccs811_start(CCS811_MODE_1SEC);

    while (1)
    {
        /*uint8_t hw_id;
         ccs811_i2cread(CCS811_HW_ID, 1, &hw_id);
         print_uart(&hw_id);*/
        //ok = I2C_masterSendSingleByteWithTimeout(EUSCI_I2C_MODULE, buf, 100);
        /*
        if(ok){
            sprintf(msg, "ok\r\n");
        }else{
            sprintf(msg, "not\r\n");
        }
        print_uart(msg);
        */
        /*
        ccs811_i2cread(CCS811_HW_ID, 1, &hw_id);
        sprintf(msg, "%d\r\n", hw_id);
        print_uart(msg);
        */
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

/*
 * @num
 */
void delay_us(uint32_t num)
{

    while (num > 0)
    {
        /*
         * MCLK = 12 MHz
         * 1200 ~ 100 us = 0,1 ms
         */
        __delay_cycles(1200);
        num--;
    }
}
