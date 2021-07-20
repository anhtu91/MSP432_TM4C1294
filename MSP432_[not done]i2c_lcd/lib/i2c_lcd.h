/********************************
 * i2c_lcd.h
 *
 *  Created on: October 11, 2018
 *  Author: Hunter Hedges
 *
 ********************************/

#ifndef I2C_LCD_H_
#define I2C_LCD_H_

/********************************
 * User Functions
 ********************************/
int LCD_init(uint8_t slaveAddress);
void LCD_send_string(char *str);
void LCD_put_cur(int row, int col);
void LCD_clear(void);

#endif /* I2C_LCD_H_ */
