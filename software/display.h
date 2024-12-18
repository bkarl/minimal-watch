#pragma once

#include "stm32l0xx_hal.h"

#define N_ROWS 2
#define N_COLUMNS 6
#define N_COLORS 2

typedef struct {
    unsigned current_column;
    uint8_t red_vals[N_ROWS];
    uint8_t green_vals[N_ROWS];
} display_state_t;

typedef struct {
    unsigned pin_idx;
    GPIO_TypeDef* gpio;
} pin_mapping_t;

extern TIM_HandleTypeDef htim2;

/*
Display has the following structure:
Row 0: LED 5 .... LED 0
Row 1: LED 5 .... LED 0
Each LED has two colors. The LSB is on the right side.
*/

#define SINK_C0_PIN GPIO_PIN_0

#define SINK_C1_PIN GPIO_PIN_2

#define SINK_C2_PIN GPIO_PIN_5

#define SINK_C3_PIN GPIO_PIN_7

#define SINK_C4_PIN GPIO_PIN_0
#define SINK_C5_PIN GPIO_PIN_8

#define RED_SOURCE_ROW_0    GPIO_PIN_1
#define RED_SOURCE_ROW_1    GPIO_PIN_1

#define GREEN_SOURCE_ROW_0    GPIO_PIN_6
#define GREEN_SOURCE_ROW_1    GPIO_PIN_3


void display_init();
void display_init_gpio();
void display_switch_all_off();
void display_iterate_column();
void switch_row_color_if_enabled(pin_mapping_t *row_pin_mapping, uint8_t row_val, unsigned row_idx);
void display_init_timer();
