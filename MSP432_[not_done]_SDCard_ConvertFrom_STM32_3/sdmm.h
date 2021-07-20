/*
 * sdmm.h
 *
 *  Created on: 10 Mar 2021
 *      Author: ATN
 */

#ifndef SDMM_H_
#define SDMM_H_

#include "ff.h"     /* Obtains integer types for FatFs */
#include "diskio.h" /* Common include file for FatFs and disk I/O layer */


/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

#include <Hardware/SPI_Driver.h>
#include <Hardware/GPIO_Driver.h>
#include <Hardware/SysTick.h>

#define SD_CS_PORT      (GPIO_PORT_P5)
#define SD_CS_PIN       (GPIO_PIN0)

#endif /* SDMM_H_ */
