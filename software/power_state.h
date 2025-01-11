#pragma once

#include "stm32l0xx_hal.h"
#include <stdbool.h>

void power_enter_stop_mode();
void power_deinit_gpios();
void power_init_timeout_counter();
void power_reset_timeout_counter();
void power_leave_stop_mode();
void power_enable_light_nfc();
void power_disable_light_nfc();

extern LPTIM_HandleTypeDef hlptim1;
extern volatile bool enter_sleep_mode;

#define GPIO_PORT_NFC_LIGHTSENSOR_SUPPLY GPIOB
#define GPIO_PIN_NFC_LIGHTSENSOR_SUPPLY GPIO_PIN_7
