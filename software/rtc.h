#pragma once

#include "stm32l0xx_hal.h"

extern RTC_HandleTypeDef hrtc;

void rtc_init(void);
void rtc_reset_time();
void rtc_set_time_from_nfc();
void rtc_set_alarm();
void rtc_enable_second_tick();
void rtc_disable_second_tick();