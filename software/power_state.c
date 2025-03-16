#include "power_state.h"
#include "settings.h"

volatile bool enter_sleep_mode = false;

void power_enter_stop_mode()
{
    //power_deinit_gpios();

    /*
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
    */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_SuspendTick();
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void power_enable_light_nfc() {
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_NFC_LIGHTSENSOR_SUPPLY;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIO_PORT_NFC_LIGHTSENSOR_SUPPLY, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIO_PORT_NFC_LIGHTSENSOR_SUPPLY, GPIO_PIN_NFC_LIGHTSENSOR_SUPPLY, GPIO_PIN_SET);
}

void power_disable_light_nfc() {
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_NFC_LIGHTSENSOR_SUPPLY;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIO_PORT_NFC_LIGHTSENSOR_SUPPLY, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIO_PORT_NFC_LIGHTSENSOR_SUPPLY, GPIO_PIN_NFC_LIGHTSENSOR_SUPPLY, GPIO_PIN_RESET);
}

void power_leave_stop_mode()
{
  HAL_ResumeTick();
  enter_sleep_mode = false;
}

void power_deinit_gpios() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_All;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
}

void power_init_timeout_counter() {
  __HAL_RCC_LPTIM1_CLK_ENABLE();

  HAL_NVIC_SetPriority(LPTIM1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
  LPTIM1->CR = 0;
  LPTIM1->IER = LPTIM_IER_CMPMIE;
  LPTIM1->CFGR = LPTIM_CFGR_PRESC;
  LPTIM1->CR = LPTIM_CR_ENABLE;
  LPTIM1->ARR = (32768 / 128) * DISPLAY_TIMEOUT_SECONDS + 1;
  LPTIM1->CMP = (32768 / 128) * DISPLAY_TIMEOUT_SECONDS;
  LPTIM1->CR = LPTIM_CR_ENABLE | LPTIM_CR_CNTSTRT;
}

void power_stop_timeout_counter() {
  LPTIM1->CR = 0;
  __HAL_RCC_LPTIM1_CLK_DISABLE();
}

void power_reset_timeout_counter() {
  LPTIM1->CR = 0;
  LPTIM1->CR = LPTIM_CR_ENABLE | LPTIM_CR_CNTSTRT;
}

void power_timeout_counter_elapsed()
{
  power_stop_timeout_counter();
  enter_sleep_mode = true;
}