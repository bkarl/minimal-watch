#pragma once

#include "stm32l0xx_hal.h"

extern RTC_HandleTypeDef hrtc;

void rtc_init(void);
void rtc_reset_time();
