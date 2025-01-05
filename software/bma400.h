#pragma once
#include "stm32l0xx_hal_conf.h"
#include "stm32l0xx_hal.h"

#define BMA400_ADDRESS                  0x28 //0b00101000
#define BMA400_CHIPID_ADDRESS           0x00
#define BMA400_ACC_INT_STAT0_ADDRESS    0x0E

#define BMA400_ACC_CONFIG0_ADDRESS      0x19
#define BMA400_ACC_CONFIG1_ADDRESS      0x1A
#define BMA400_ACC_CONFIG2_ADDRESS      0x1B
#define BMA400_ACC_STEP_CNT0_ADDRESS    0x15
#define BMA400_INT_CONFIG0_ADDRESS      0x1F
#define BMA400_INT_CONFIG1_ADDRESS      0x20
#define BMA400_INT1_MAP_ADDRESS         0x21
#define BMA400_INT2_MAP_ADDRESS         0x22
#define BMA400_INT12_MAP_ADDRESS        0x23
#define BMA400_INT12_IO_CTRL_ADDRESS    0x24

#define BMA400_ACC_CONFIG0_DATA         0x02 //enable normal mode
#define BMA400_ACC_CONFIG1_DATA         0x19 //acc_range = 0, osr = 1, acc_odr = 200 Hz
#define BMA400_ACC_CONFIG2_DATA         0x00 //data_src_reg = variable ODR filter

#define BMA400_INT_CONFIG0_DATA         0x00
#define BMA400_INT_CONFIG1_DATA         0x08 //d_tap_int_en, non latching mode
#define BMA400_INT_MAP1_DATA            0x00
#define BMA400_INT_MAP2_DATA            0x00
#define BMA400_INT_MAP12_DATA           0x40 //tap sensing interrupt is mapped to INT2
#define BMA400_INT12_IO_CTRL_DATA       0x20 //interrupt pin INT2 high-active, pp

uint8_t bma400_get_chip_id();
void bma400_init();