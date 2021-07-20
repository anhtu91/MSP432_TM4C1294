/*
 * UART_Transmit.c
 *
 *  Created on: 28 Mar 2021
 *      Author: ATN
 */
#include "UART_Transmit.h"

void print_uart(char *msg)
{
    uint32_t i;

    for (i = 0; i < strlen(msg); i++)
    {
        UART_transmitData(EUSCI_A0_BASE, msg[i]);
    }
}
