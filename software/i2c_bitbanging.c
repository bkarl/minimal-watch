#include "i2c_bitbanging.h"

TIM_HandleTypeDef htim21;

void i2c_init() {
    i2c_init_gpio();
    i2c_init_delay_timer();
}

void i2c_read_register(uint8_t address, uint8_t reg_address, uint8_t* dout, uint8_t n_read) {
    i2c_start_condition();
    i2c_master_write(address | 1);
    i2c_master_write(reg_address);
    i2c_start_condition();
    for (int i = 0; i < n_read; i++) {
        uint8_t ack = i == n_read - 1 ? 0 : 1;
        dout[i] = i2c_master_read(ack);
    }
    i2c_stop_condition();
}

void i2c_write_register(uint8_t address, uint8_t reg_address, uint8_t* din, uint8_t n_write) {
    i2c_start_condition();
    i2c_master_write(address | 1);
    i2c_master_write(reg_address);
    for (int i = 0; i < n_write; i++) {
        i2c_master_write(din[i]);
    }
    i2c_stop_condition();
}
/**
* @brief I2C master write 8-bit data bit-bang
* @param unsigned char b - data to transmit
* @retval unsigned char ack – acknowledgement received
*/
uint8_t i2c_master_write (unsigned char b)
{
    unsigned char msk = 0x80;
    unsigned char ack;
    do
    {
        (b & msk) ? i2c_set_sda() : i2c_clear_sda();
        i2c_half_bit_delay();
        i2c_set_scl();
        i2c_half_bit_delay();
        i2c_clear_scl();
    }
    while ((msk>>=1) != 0);
    i2c_set_sda();/* ACK slot checking */
    i2c_set_scl();
    i2c_half_bit_delay();
    ack = i2c_get_sda();
    i2c_clear_scl();
    return (ack);
}

/**
* @brief I2C master read 8-bit bit-bang
* @param unsigned char ack – acknowledgement control
* @retval unsigned char b – data received
*/
unsigned char i2c_master_read (unsigned char ack)
{
    unsigned char msk = 0x80;
    unsigned char b = 0;
    do
    {
        i2c_set_scl();
        i2c_half_bit_delay();
        if(i2c_get_sda() != 0)
            b |= msk;
        i2c_clear_scl();
        i2c_half_bit_delay();
    }
    while ((msk>>=1) != 0);
    if(ack != 0)
    {
        i2c_clear_sda();/* ACK slot control */
    }
    i2c_set_scl();
    i2c_half_bit_delay();
    i2c_clear_scl();
    i2c_half_bit_delay();
    return (b);
}

/**
* @brief I2C start
* @param none
* @retval none
*/
void i2c_start_condition(void)
{
    i2c_half_bit_delay();
    i2c_clear_sda();
    i2c_half_bit_delay();
    i2c_clear_scl();
    i2c_half_bit_delay();
}
/**
* @brief I2C stop
* @param none
* @retval none
*/
void i2c_stop_condition(void)
{
    i2c_clear_sda();
    i2c_clear_scl();
    i2c_half_bit_delay();
    i2c_set_scl();
    i2c_half_bit_delay();
    i2c_set_sda();
    i2c_half_bit_delay();
}

void i2c_init_gpio() {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void i2c_init_delay_timer() {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim21.Instance = TIM21;
    htim21.Init.Prescaler = 24; //1 MHz clock rate
    htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim21.Init.Period = 1;
    htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim21);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig);

    HAL_TIM_Base_Start(&htim21);
}

void i2c_half_bit_delay() {
    __HAL_TIM_SET_COUNTER(&htim21,0);  
	while (__HAL_TIM_GET_COUNTER(&htim21) < 1);
}

void i2c_set_sda() { HAL_GPIO_WritePin(I2C_DO_PORT, I2C_SDA_PIN, GPIO_PIN_SET); }
void i2c_clear_sda() { HAL_GPIO_WritePin(I2C_DO_PORT, I2C_SDA_PIN, GPIO_PIN_RESET); }
uint8_t i2c_get_sda() { return I2C_DIN_PORT & I2C_SDA_PIN; }
void i2c_set_scl() { HAL_GPIO_WritePin(I2C_DO_PORT, I2C_SCL_PIN, GPIO_PIN_SET); }
void i2c_clear_scl() { HAL_GPIO_WritePin(I2C_DO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET); }
