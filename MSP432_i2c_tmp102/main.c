/***
 * Read TMP102 via I2C
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define EUSCI_I2C_MODULE                EUSCI_B1_BASE
#define EUSCI_I2C_STATUS_SLAVE_NACK     1

static const uint8_t TMP102_ADDR = 0b1001000;
static const uint8_t REG_TEMP = 0x00;

static void print_uart(char *msg);

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

    uint8_t buf[12];
    int16_t val;
    float temp_c;

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
    I2C_setSlaveAddress(EUSCI_I2C_MODULE, TMP102_ADDR);
    I2C_enableModule(EUSCI_I2C_MODULE);

    print_uart("Start...\r\n");

    while (1)
    {
        I2C_masterSendSingleByteWithTimeout(EUSCI_I2C_MODULE, REG_TEMP, 5000);

        I2C_masterReceiveStart(EUSCI_I2C_MODULE);

        buf[0] = I2C_masterReceiveMultiByteNext(EUSCI_I2C_MODULE);
        buf[1] = I2C_masterReceiveMultiByteNext(EUSCI_I2C_MODULE);

        I2C_masterReceiveMultiByteStop(EUSCI_I2C_MODULE);

        val = ((int16_t) buf[0] << 4 | (buf[1] >> 4));
        //Convert to two complement, temp could be negative
        if (val > 0x7FF)
        {
            val |= 0xF000;
        }
        //Convert to float temperature value (C)
        temp_c = val * 0.0625;

        temp_c *= 100;

        sprintf((char*) buf, "%u.%u C\r\n", ((unsigned int) temp_c / 100),
                ((unsigned int) temp_c % 100));

        print_uart((char*) buf);

        memset(buf, 0, sizeof buf);
        __delay_cycles(1200000);
    }
}

static void print_uart(char *msg)
{
    uint32_t i;

    for (i = 0; i < strlen(msg); i++)
    {
        UART_transmitData(EUSCI_A0_BASE, msg[i]);
    }
}

