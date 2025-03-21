#include "display.h"
#include "rtc.h"
#include "main.h"

display_state_t display_state;

pin_mapping_t column_pin_mapping[] = {
    {GPIO_PIN_8, GPIOA},
    {GPIO_PIN_0, GPIOB},
    {GPIO_PIN_7, GPIOA},
    {GPIO_PIN_5, GPIOA},
    {GPIO_PIN_2, GPIOA},
    {GPIO_PIN_0, GPIOA},
};

pin_mapping_t row_pin_mapping_red[] = {
    {GPIO_PIN_1, GPIOA},
    {GPIO_PIN_1, GPIOB},
};

pin_mapping_t row_pin_mapping_green[] = {
    {GPIO_PIN_6, GPIOA},
    {GPIO_PIN_3, GPIOA}
};

void display_init() {
    display_state.current_column = 0;
    display_init_gpio();
    display_init_timer();
    display_set_time();
    rtc_enable_second_tick();
}

void display_shutdown() {
    TIM2->CR1 = 0;
    display_switch_all_off();
    __HAL_RCC_TIM2_CLK_DISABLE();
    rtc_disable_second_tick();
}

void display_set_time()
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    switch (global_state.display_mode) {
        case DISPLAY_MODE_TIME: 
            display_state.red_vals[0] = sTime.Hours;
            display_state.green_vals[1] = sTime.Minutes;
            break;
        case DISPLAY_MODE_SECONDS: 
            display_state.red_vals[0] = sTime.Minutes;
            display_state.green_vals[1] = sTime.Seconds;
            break;
        case DISPLAY_MODE_STEPS: {
                if (global_state.step_ctr_val_today > STEP_PER_DAY_TARGET && global_state.step_ctr_val_today < 2 * STEP_PER_DAY_TARGET) {
                    display_set_step_ctr(display_state.green_vals);
                }
                else if (global_state.step_ctr_val_today >= 2 * STEP_PER_DAY_TARGET) {
                    display_set_step_ctr(display_state.red_vals);
                    display_set_step_ctr(display_state.green_vals);
                }
                else {
                    display_set_step_ctr(display_state.red_vals);
                }
                break;
            }
    }
}

void display_set_step_ctr(uint8_t *pixel_reg) {
    for (int i = 0; i < N_COLUMNS; i++) {
        for (int row = 0; row < N_ROWS; row++) {
            uint8_t on_off = 0;
            if (global_state.step_ctr_val_today / (STEPS_PER_TICK * (i + row * N_COLUMNS)) >= 0.5)
                on_off = 1;
            pixel_reg[i] = on_off << (N_COLUMNS - 1 - i);
        }           
    }
    if (pixel_reg[0] == 0)
        pixel_reg[0] = 1; //dont show a blank display
}

void display_update() {
    display_iterate_column();
}

void display_iterate_column() {
    display_switch_all_off();
    for (int row_idx = 0; row_idx < N_ROWS; row_idx++) {
        switch_row_color_if_enabled(row_pin_mapping_red, display_state.red_vals[row_idx], row_idx);
        switch_row_color_if_enabled(row_pin_mapping_green, display_state.green_vals[row_idx], row_idx);
    }
    HAL_GPIO_WritePin(column_pin_mapping[display_state.current_column].gpio, column_pin_mapping[display_state.current_column].pin_idx, GPIO_PIN_RESET);
    display_state.current_column = (display_state.current_column + 1) % N_COLUMNS;
}

void switch_row_color_if_enabled(pin_mapping_t *row_pin_mapping, uint8_t row_val, unsigned row_idx) {
    if (row_val & (1 << display_state.current_column)) {
        HAL_GPIO_WritePin(row_pin_mapping[row_idx].gpio, row_pin_mapping[row_idx].pin_idx, GPIO_PIN_SET);
    }
}

void display_switch_all_off() {
    HAL_GPIO_WritePin(GPIOA, RED_SOURCE_ROW_0 | GREEN_SOURCE_ROW_0 | GREEN_SOURCE_ROW_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, RED_SOURCE_ROW_1, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOA, SINK_C0_PIN | SINK_C1_PIN | SINK_C2_PIN | SINK_C3_PIN | SINK_C5_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, SINK_C4_PIN, GPIO_PIN_SET);
}

void display_init_gpio() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

    display_switch_all_off();

  //configure sources
  GPIO_InitStruct.Pin = RED_SOURCE_ROW_0 | GREEN_SOURCE_ROW_0 | GREEN_SOURCE_ROW_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RED_SOURCE_ROW_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  //configure sinks
  GPIO_InitStruct.Pin = SINK_C0_PIN | SINK_C1_PIN | SINK_C2_PIN | SINK_C3_PIN | SINK_C5_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SINK_C4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void display_init_timer()
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM2->DIER = TIM_DIER_UIE_Msk;
    TIM2->SMCR = 0;
    TIM2->PSC = 2400;
    TIM2->ARR = 10;
    TIM2->EGR = TIM_EGR_UG;

    TIM2->CR1 = TIM_CR1_CEN_Msk;
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc) {
    UNUSED(hrtc);
    display_set_time();
}
