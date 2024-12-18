#include "display.h"

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
    {GPIO_PIN_3, GPIOB}
};

TIM_HandleTypeDef htim2;

void display_init() {
    display_state.current_column = 0;
    display_init_gpio();
    display_init_timer();
}

void display_set_time()
{
    
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    UNUSED(htim);
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
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 24000;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 10;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    HAL_TIM_Base_Start_IT(&htim2);
}