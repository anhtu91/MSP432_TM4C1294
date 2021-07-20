// SET THIS TO THE CORRECT CLOCK FREQUENCY
// Defaulted to 3MHz (MSP432 default)
#define CLOCK_FREQ  3000000

/********************************
 * Includes
 ********************************/
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "i2c_lcd.h"

/********************************
 * File specific functions
 ********************************/
static void _delayInit(void);
static void _delayMicroseconds(uint32_t durationUs);
static void _i2cInit(uint8_t slaveAddress);
static void lcd_send_cmd(char cmd);
static void lcd_send_data(char data);


// I2C Master Configuration
const eUSCI_I2C_MasterConfig i2cConfig = {
EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        CLOCK_FREQ,                             // Set the Clock Frequency
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100KHz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
        };

/********************************
 * We want to use the TIMER32 on the MSP432
 * because this will allow us to use the same
 * code with different clock frequencies easily
 *
 * Changes made to the TIMER32 could affect the LCD
 ********************************/
static void _delayInit(void)
{
    Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1,
    TIMER32_32BIT,
                       TIMER32_PERIODIC_MODE);
}

static void _delayMicroseconds(uint32_t durationUs)
{
    durationUs = durationUs * (CLOCK_FREQ / 1000000);

    Timer32_setCount(TIMER32_BASE, durationUs);
    Timer32_startTimer(TIMER32_BASE, true);

    while (Timer32_getValue(TIMER32_BASE) != 0)
        ;

    Timer32_haltTimer(TIMER32_BASE);
}

/********************************
 * Initializes the I2C EUSCI_B0 on the MSP432
 * Pin 6.4 = SDA (Data)
 * Pin 6.5 = SCL (Clock)
 ********************************/
static void _i2cInit(uint8_t slaveAddress)
{
    // Selects Port 1 for I2c (6.4 = SDA, 6.5 = SCL)
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
    GPIO_PIN4 + GPIO_PIN5,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    // Initialize based on config
    I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);

    // Specify slave address of LCD
    I2C_setSlaveAddress(EUSCI_B1_BASE, slaveAddress);

    // Set master in transmit mode
    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Enable to start operation
    I2C_enableModule(EUSCI_B1_BASE);
}

static void lcd_send_cmd(char cmd)
{
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C;  //en=1, rs=0
    data_t[1] = data_u | 0x08;  //en=0, rs=0
    data_t[2] = data_l | 0x0C;  //en=1, rs=0
    data_t[3] = data_l | 0x08;  //en=0, rs=0

    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[0]);
    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[1]);
    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[2]);
    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[3]);
}

static void lcd_send_data(char data)
{
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D;  //en=1, rs=0
    data_t[1] = data_u | 0x09;  //en=0, rs=0
    data_t[2] = data_l | 0x0D;  //en=1, rs=0
    data_t[3] = data_l | 0x09;  //en=0, rs=0

    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[0]);
    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[1]);
    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[2]);
    I2C_masterSendSingleByte(EUSCI_B1_BASE, data_t[3]);
}

void LCD_clear(void)
{
    int i;
    lcd_send_cmd(0x80);

    for (i = 0; i < 70; i++)
    {
        lcd_send_data(' ');
    }
}

void LCD_put_cur(int row, int col)
{
    switch (row)
    {
    case 0:
        col |= 0x80;
        break;
    case 1:
        col |= 0xC0;
        break;
    }
    lcd_send_cmd(col);
}

void LCD_send_string(char *str)
{
    while (*str)
        lcd_send_data(*str++);
}

/********************************
 *
 * Param: Slave address of LCD
 * Returns: 1 on success, 0 otherwise
 ********************************/
int LCD_init(uint8_t slaveAddress)
{
    // Make sure the CLOCK_FREQ definition matches actual clock frequency
    uint32_t clockFreq = CS_getSMCLK();
    if (CLOCK_FREQ != clockFreq)
    {
        // Failed
        return 0;
    }

    _i2cInit(slaveAddress);
    _delayInit();

    // We need at least 40ms after power rises above 2.7V
    _delayMicroseconds(50 * 1000);

    // Now we pull both RS and R/W low to begin commands
    lcd_send_cmd(0x30);

    _delayMicroseconds(5 * 1000);

    lcd_send_cmd(0x30);

    _delayMicroseconds(1 * 1000);

    lcd_send_cmd(0x30);

    _delayMicroseconds(10 * 1000);

    lcd_send_cmd(0x20);

    _delayMicroseconds(10 * 1000);

    // dislay initialisation
    lcd_send_cmd(0x28);

    _delayMicroseconds(1 * 1000);

    lcd_send_cmd(0x08);

    _delayMicroseconds(1 * 1000);

    lcd_send_cmd(0x01);

    _delayMicroseconds(1 * 1000);
    _delayMicroseconds(1 * 1000);

    lcd_send_cmd(0x06);

    _delayMicroseconds(1 * 1000);

    lcd_send_cmd(0x0C);

    return 1;
}
