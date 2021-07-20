/***
 * Sample project
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <Hardware/CS_Driver.h>
#include <Devices/MSPIO.h>
#include <Devices/ESP8266.h>

/* VCC = 3.3V
 * GND = GND
 * TX = P3.2
 * RX = P3.3
 * RST = P6.1
 * CH_PD = 3.3V */


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

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_ConfigV1 UART2Config = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 13, 0, 37,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION };

void main(void)
{
    MAP_WDT_A_holdTimer();

    /*MSP432 Running at 24 MHz*/
    CS_Init();

    /*Initialize required hardware peripherals for the ESP8266*/
    UART_Init(EUSCI_A0_BASE, UART0Config);
    UART_Init(EUSCI_A2_BASE, UART2Config);
    GPIO_Init(GPIO_PORT_P6, GPIO_PIN1);

    MAP_Interrupt_enableMaster();

    /*Pointer to ESP8266 global buffer*/
    char *ESP8266_Data = ESP8266_GetBuffer();

    /*Hard Reset ESP8266*/
    ESP8266_HardReset();
    __delay_cycles(48000000);
    /*flush reset data, we do this because a lot of data received cannot be printed*/
    UART_Flush(EUSCI_A2_BASE);

    /*Check UART connection to MSP432*/
    if (!ESP8266_CheckConnection())
    {
        MSPrintf(EUSCI_A0_BASE, "Failed MSP432 UART connection\r\n");
        /*Trap MSP432 if failed connection*/
        while (1)
            ;
    }

    MSPrintf(EUSCI_A0_BASE, "Nice! We are connected to the MSP432\r\n\r\n");
    MSPrintf(EUSCI_A0_BASE, "Checking available Access Points\r\n\r\n");

    /*Check available Access Points*/
    if (!ESP8266_AvailableAPs())
    {
        MSPrintf(EUSCI_A0_BASE,
                 "Failed to obtain Access Points\n\r ERROR: %s \r\n",
                 ESP8266_Data);
        //Trap MSP432 if failed to obtain access points
        while (1)
            ;
    }

    /*Print received Access Points Data to console*/
    MSPrintf(EUSCI_A0_BASE,
             "Got it! Here are the available Access Points: %s\r\n\r\n",
             ESP8266_Data);

    /*Connect to Access Point if necessary here*/
    if (!ESP8266_ConnectToAP("Wireless Network", "greenzoo@150"))
    {
        MSPrintf(EUSCI_A0_BASE, "No Wifi Connection\r\n\r\n");
    }else{
        MSPrintf(EUSCI_A0_BASE, "Connected Wifi\r\n\r\n");
    }


    if(!ESP8266_EnableMultipleConnections(false)){
        MSPrintf(EUSCI_A0_BASE, "Enable single connection not successful\r\n\r\n");
    }else{
        MSPrintf(EUSCI_A0_BASE, "Enable single connection successful\r\n\r\n");
    }

    //TCP = 0, UDP = 1
    if(!ESP8266_EstablishConnection(0, "192.168.1.9", 1883)){
        MSPrintf(EUSCI_A0_BASE, "No Connection to MQTT Server\r\n\r\n");
    }else{
        MSPrintf(EUSCI_A0_BASE, "Connected MQTT Server\r\n\r\n");
    }


    //char msg[] = {0x10, 0x11, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0x02, 0x00, 0x3C, 0x00, 0x05, 0x49, 0x4F, 0x54, 0x34, 0x37};
    char *msg = "0x10 0x11 0x00 0x04 0x4D 0x51 0x54 0x54 0x04 0x02 0x00 0x3C 0x00 0x05 0x49 0x4F 0x54 0x34 0x37";

    if(ESP8266_SendData(msg, 19)){
        MSPrintf(EUSCI_A0_BASE, "Send MQTT successful\r\n\r\n");
    }else{
        MSPrintf(EUSCI_A0_BASE, "Send MQTT NOT successful\r\n\r\n");
    }


    while (1)
    {
        /*You can query data here by sending the HTTP request string every n seconds.
         * Be careful about the amount of times per second that you can query data from an API
         */
    }
}

