#pragma once
#include "stm32l0xx_hal_conf.h"

#define BMA400_ADDRESS 0b00101000
#define BMA400_CHIPID_ADDRESS 0

uint8_t bma400_get_chip_id();