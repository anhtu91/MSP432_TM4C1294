/*
 * ccs811.c
 *
 *  Created on: 13 Jan 2021
 *      Author: ATN
 */
#include <lib/ccs811.h>

bool ccs811_begin(void);
bool ccs811_i2cwrite(uint8_t regaddr, uint8_t count, const uint8_t *buf);
void ccs811_i2cread(uint8_t regaddr, uint8_t count, uint8_t *buf);
bool ccs811_start(int mode);
void print_uart(char *msg);
void delay_us(uint32_t us);
int hardware_version(void);

bool ccs811_begin(void)
{
    uint8_t sw_reset[] = { 0x11, 0xE5, 0x72, 0x8A };
    uint8_t app_start[] = { };
    uint8_t hw_id;
    uint8_t hw_version;
    uint8_t app_version[2];
    uint8_t status;
    bool ok;

    ok = ccs811_i2cwrite(0, 0, 0);
    /*
     if (!ok)
     {
     ok = ccs811_i2cwrite(0, 0, 0);
     }

     if (ok)
     {
     print_uart("Ping to ccs811 ok\r\n");
     }
     else
     {
     print_uart("Ping to ccs811 failed\r\n");
     return false;
     }
     */
    ok = ccs811_i2cwrite(CCS811_SW_RESET, 4, sw_reset);

    /*
     if (ok)
     {
     print_uart("Reset ccs811 ok\r\n");
     }
     else
     {
     print_uart("Reset ccs811 failed\r\n");
     //goto abort_begin;
     }
     */
    delay_us(200); //delay 2000us ~ 2ms

    ccs811_i2cread(CCS811_HW_ID, 1, &hw_id);
    ccs811_i2cread(CCS811_HW_ID, 1, &hw_id);
    ccs811_i2cread(CCS811_HW_ID, 1, &hw_id);
    if (hw_id != 0x81)
    {
        print_uart("HW_ID failed\r\n");
        return false;
    }
    else
    {
        print_uart("HW_ID ok\r\n");
    }

    ccs811_i2cread(CCS811_HW_VERSION, 1, &hw_version);
    if ((hw_version & 0xF0) != 0x10)
    {
        print_uart("ccs811: Wrong HW_VERSION \r\n");
        //goto abort_begin;
    }
    else
    {
        print_uart("ccs811: Correct HW_VERSION \r\n");
    }

    /*
     ccs811_i2cread(CCS811_STATUS, 1, &status);
     if (status != 0x10)
     {
     print_uart("ccs811: Not in boot mode, or no valid app \r\n");
     goto abort_begin;
     }
     */
    /*
     ccs811_i2cread(CCS811_FW_APP_VERSION, 2, app_version);

     ccs811_i2cread(CCS811_APP_START, 0, app_start);
     delay_us(10); //delay 1000us

     ccs811_i2cread(CCS811_STATUS, 1, &status);
     if (status != 0x90)
     {
     print_uart("ccs811: Not in app mode, or no valid app \r\n");
     }
     */
    return true;
}

bool ccs811_i2cwrite(uint8_t regaddr, uint8_t count, const uint8_t *buf)
{
    //bool ok = false;
    int i = 0;

    I2C_setSlaveAddress(EUSCI_B1_BASE, CCS811_SLAVEADDR_0);

    //I2C_masterSendStart(EUSCI_B1_BASE);
    I2C_masterSendSingleByteWithTimeout(EUSCI_B1_BASE, regaddr, 200);

    for (i = 0; i < count; i++)
    {
        if (i == 0)
        {
            I2C_masterSendMultiByteStartWithTimeout(EUSCI_B1_BASE, buf[i], 200);
        }
        else if (i == count - 1)
        {
            I2C_masterSendMultiByteFinishWithTimeout(EUSCI_B1_BASE, buf[i],
                                                     200);
        }
        else
        {
            I2C_masterSendMultiByteNextWithTimeout(EUSCI_B1_BASE, buf[i], 200);
        }
    }

    if (I2C_masterIsStopSent(EUSCI_B1_BASE) == EUSCI_B_I2C_STOP_SEND_COMPLETE)
    {
        return true;
    }
    return false;
}

void ccs811_i2cread(uint8_t regaddr, uint8_t count, uint8_t *buf)
{
    int i = 0;

    I2C_setSlaveAddress(EUSCI_B1_BASE, CCS811_SLAVEADDR_0);

    I2C_masterSendSingleByteWithTimeout(EUSCI_B1_BASE, regaddr, 200);

    I2C_masterReceiveStart(EUSCI_B1_BASE);

    for (i = 0; i < count; i++)
    {
        buf[i] = I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
    }

    I2C_masterReceiveMultiByteStop(EUSCI_B1_BASE);
}

bool ccs811_start(int mode)
{
    uint8_t meas_mode[] = { (uint8_t) (mode << 4) };
    bool ok = ccs811_i2cwrite(CCS811_MEAS_MODE, 1, meas_mode);
    return ok;
}

int hardware_version(void)
{
    uint8_t buf[1];

    ccs811_i2cread(CCS811_HW_VERSION, 1, buf);

    int version = -1;
    version = buf[0];

    return version;
}
