#pragma once

#include "stm32l0xx_hal.h"
#include <stdbool.h>

void power_enter_stop_mode();
void power_deinit_gpios();
void power_init_timeout_counter();
void power_reset_timeout_counter();
void power_leave_stop_mode();

extern LPTIM_HandleTypeDef hlptim1;
extern volatile bool enter_sleep_mode;
