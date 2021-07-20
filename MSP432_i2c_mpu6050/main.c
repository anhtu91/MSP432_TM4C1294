/***
 * Sample project
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define UART_PORT       GPIO_PORT_P1
#define UART_TX         GPIO_PIN2
#define UART_RX         GPIO_PIN3

#define I2C_PORT        GPIO_PORT_P6
#define I2C_SDA_PIN     GPIO_PIN4
#define I2C_SCL_PIN     GPIO_PIN5
#define I2C_Module      EUSCI_B1_BASE

#define MPU6050_ADDR                  0b1101000
#define MPU6050_WHO_AM_I_REG          (0x75)

#define MPU6050_REG_ACCEL_XOFFS_H     (0x06)
#define MPU6050_REG_ACCEL_XOFFS_L     (0x07)
#define MPU6050_REG_ACCEL_YOFFS_H     (0x08)
#define MPU6050_REG_ACCEL_YOFFS_L     (0x09)
#define MPU6050_REG_ACCEL_ZOFFS_H     (0x0A)
#define MPU6050_REG_ACCEL_ZOFFS_L     (0x0B)
#define MPU6050_REG_GYRO_XOFFS_H      (0x13)
#define MPU6050_REG_GYRO_XOFFS_L      (0x14)
#define MPU6050_REG_GYRO_YOFFS_H      (0x15)
#define MPU6050_REG_GYRO_YOFFS_L      (0x16)
#define MPU6050_REG_GYRO_ZOFFS_H      (0x17)
#define MPU6050_REG_GYRO_ZOFFS_L      (0x18)
#define MPU6050_REG_CONFIG            (0x1A)
#define MPU6050_REG_GYRO_CONFIG       (0x1B) // Gyroscope Configuration
#define MPU6050_REG_ACCEL_CONFIG      (0x1C) // Accelerometer Configuration
#define MPU6050_REG_FF_THRESHOLD      (0x1D)
#define MPU6050_REG_FF_DURATION       (0x1E)
#define MPU6050_REG_MOT_THRESHOLD     (0x1F)
#define MPU6050_REG_MOT_DURATION      (0x20)
#define MPU6050_REG_ZMOT_THRESHOLD    (0x21)
#define MPU6050_REG_ZMOT_DURATION     (0x22)
#define MPU6050_REG_INT_PIN_CFG       (0x37) // INT Pin. Bypass Enable Configuration
#define MPU6050_REG_INT_ENABLE        (0x38) // INT Enable
#define MPU6050_REG_INT_STATUS        (0x3A)
#define MPU6050_REG_ACCEL_XOUT_H      (0x3B)
#define MPU6050_REG_ACCEL_XOUT_L      (0x3C)
#define MPU6050_REG_ACCEL_YOUT_H      (0x3D)
#define MPU6050_REG_ACCEL_YOUT_L      (0x3E)
#define MPU6050_REG_ACCEL_ZOUT_H      (0x3F)
#define MPU6050_REG_ACCEL_ZOUT_L      (0x40)
#define MPU6050_REG_TEMP_OUT_H        (0x41)
#define MPU6050_REG_TEMP_OUT_L        (0x42)
#define MPU6050_REG_GYRO_XOUT_H       (0x43)
#define MPU6050_REG_GYRO_XOUT_L       (0x44)
#define MPU6050_REG_GYRO_YOUT_H       (0x45)
#define MPU6050_REG_GYRO_YOUT_L       (0x46)
#define MPU6050_REG_GYRO_ZOUT_H       (0x47)
#define MPU6050_REG_GYRO_ZOUT_L       (0x48)
#define MPU6050_REG_MOT_DETECT_STATUS (0x61)
#define MPU6050_REG_MOT_DETECT_CTRL   (0x69)
#define MPU6050_REG_USER_CTRL         (0x6A) // User Control
#define MPU6050_REG_PWR_MGMT_1        (0x6B) // Power Management 1
#define MPU6050_SMPLRT_DIV            (0x19)

static void print_uart(char *msg);
static void MPU6050_Begin(void);

const eUSCI_I2C_MasterConfig i2cConfig = {
EUSCI_B_I2C_CLOCKSOURCE_SMCLK,                  // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
        };

const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 78, 2, 0, EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT, EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
        EUSCI_A_UART_8_BIT_LEN };

int main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(UART_PORT,
    UART_TX | UART_RX,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Set function i2c for port */
    GPIO_setAsPeripheralModuleFunctionInputPin(I2C_PORT,
    I2C_SDA_PIN | I2C_SCL_PIN,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    I2C_initMaster(I2C_Module, &i2cConfig);
    I2C_setSlaveAddress(I2C_Module, MPU6050_ADDR);
    I2C_enableModule(I2C_Module);

    MPU6050_Begin();
    char msg[50];
    float Xa, Ya, Za, temp, Xg, Yg, Zg;
    uint16_t xAxis, yAxis, zAxis, t, xgAxis, ygAxis, zgAxis;
    int xha, xla, yha, yla, zha, zla, tmp1, tmp2, gx1, gx2, gy1, gy2, gz1, gz2;

    while (1)
    {

        I2C_masterSendSingleByteWithTimeout(I2C_Module, MPU6050_REG_GYRO_XOUT_H,
                                            200);
        while (I2C_masterIsStopSent(I2C_Module)
                != EUSCI_B_I2C_STOP_SEND_COMPLETE)
            ;

        I2C_masterReceiveStart(I2C_Module);

        xha = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        xla = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        yha = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        yla = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        zha = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        zla = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        tmp1 = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        tmp2 = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        gx1 = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        gx2 = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        gy1 = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        gy2 = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        gz1 = (int) I2C_masterReceiveMultiByteNext(I2C_Module);
        gz2 = (int) I2C_masterReceiveMultiByteFinish(I2C_Module);

        xAxis = xha << 8 | xla;
        yAxis = yha << 8 | yla;
        zAxis = zha << 8 | zla;

        xgAxis = gx1 << 8 | gx2;
        ygAxis = gy1 << 8 | gy2;
        zgAxis = gz1 << 8 | gz2;

        t = tmp1 << 8 | tmp2;

        Xa = (float) xAxis / 16384.0;
        Ya = (float) yAxis / 16384.0;
        Za = (float) zAxis / 16384.0;
        Xg = (float) xgAxis / 131.0;
        Yg = (float) ygAxis / 131.0;
        Zg = (float) zgAxis / 131.0;
        temp = ((float) t / 340.00) + 36.53;/* Convert temperature in °/c */

        sprintf(msg, "X: %.2f, Y: %.2f, Z: %.2f \r\n", Xa, Ya, Za);
        print_uart(msg);
        //sprintf(msg, "Gx: %.2f, Gy: %.2f, Gz: %.2f \r\n", Xg, Yg, Zg);
        //print_uart(msg);
        //sprintf(msg, "Temp : %.2f\r\n", temp);
        //print_uart(msg);

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

static void MPU6050_Begin(void)
{
    // Setting The Sample (Data) Rate
    I2C_setSlaveAddress(I2C_Module, MPU6050_ADDR);
    I2C_masterSendMultiByteStart(I2C_Module, MPU6050_SMPLRT_DIV);
    I2C_masterSendMultiByteFinish(I2C_Module, 0x07);

    while (I2C_masterIsStopSent(I2C_Module) != EUSCI_B_I2C_STOP_SEND_COMPLETE)
        ;

    // Setting The Clock Source
    I2C_setSlaveAddress(I2C_Module, MPU6050_ADDR);
    I2C_masterSendMultiByteStart(I2C_Module, MPU6050_REG_PWR_MGMT_1);
    I2C_masterSendMultiByteFinish(I2C_Module, 0x01);

    while (I2C_masterIsStopSent(I2C_Module) != EUSCI_B_I2C_STOP_SEND_COMPLETE)
        ;

    // Configure The DLPF
    I2C_setSlaveAddress(I2C_Module, MPU6050_ADDR);
    I2C_masterSendMultiByteStart(I2C_Module, MPU6050_REG_CONFIG);
    I2C_masterSendMultiByteFinish(I2C_Module, 0x00);

    while (I2C_masterIsStopSent(I2C_Module) != EUSCI_B_I2C_STOP_SEND_COMPLETE)
        ;

    // Configure The ACCEL (FSR= +-2g)
    I2C_setSlaveAddress(I2C_Module, MPU6050_ADDR);
    I2C_masterSendMultiByteStart(I2C_Module, MPU6050_REG_ACCEL_CONFIG);
    I2C_masterSendMultiByteFinish(I2C_Module, 0x00);

    while (I2C_masterIsStopSent(I2C_Module) != EUSCI_B_I2C_STOP_SEND_COMPLETE)
        ;

    // Configure The GYRO (FSR= +-2000d/s)
    I2C_setSlaveAddress(I2C_Module, MPU6050_ADDR);
    I2C_masterSendMultiByteStart(I2C_Module, MPU6050_REG_GYRO_CONFIG);
    I2C_masterSendMultiByteFinish(I2C_Module, 0x18);

    while (I2C_masterIsStopSent(I2C_Module) != EUSCI_B_I2C_STOP_SEND_COMPLETE)
        ;

    // Enable Data Ready Interrupts
    I2C_setSlaveAddress(I2C_Module, MPU6050_ADDR);
    I2C_masterSendMultiByteStart(I2C_Module, MPU6050_REG_INT_ENABLE);
    I2C_masterSendMultiByteFinish(I2C_Module, 0x01);

    while (I2C_masterIsStopSent(I2C_Module) != EUSCI_B_I2C_STOP_SEND_COMPLETE)
        ;
}

