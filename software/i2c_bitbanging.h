#pragma once

#include "stm32l0xx_hal_conf.h"

#define I2C_SDA_PIN GPIO_PIN_4
#define I2C_SCL_PIN GPIO_PIN_10
#define I2C_DIN_PORT GPIOA->IDR
#define I2C_DO_PORT GPIOA

void i2c_set_sda();
void i2c_clear_sda();
uint8_t i2c_get_sda();
void i2c_set_scl();
void i2c_clear_scl();
void i2c_half_bit_delay();
uint8_t i2c_master_write (unsigned char b);
unsigned char i2c_master_read (unsigned char ack);
void i2c_start_condition(void);
void i2c_stop_condition(void);
void i2c_init_gpio();
void i2c_init_delay_timer();
void i2c_init();
void i2c_read_register(uint8_t address, uint8_t reg_address, uint8_t* dout, uint8_t n_read);
void i2c_write_register(uint8_t address, uint8_t reg_address, uint8_t* din, uint8_t n_write);