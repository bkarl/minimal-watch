#include "interrupt.h"
#include "rtc.h"

volatile uint8_t wakeup_reason;

void interrupts_init() {
    wakeup_reason = WAKEUP_REASON_NONE;
    interrupts_init_gpio();
    HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void interrupts_init_gpio() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(BMA_400_INT_GPIO, BMA_400_INT1_PIN | BMA_400_INT2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(NFC_INT_GPIO, NFC_INT_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUTTON_INT_GPIO, BUTTON_0_PIN | BUTTON_1_PIN | BUTTON_2_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = BMA_400_INT1_PIN | BMA_400_INT2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BMA_400_INT_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = NFC_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(NFC_INT_GPIO, &GPIO_InitStruct);
    HAL_GPIO_ReadPin(NFC_INT_GPIO, NFC_INT_PIN);
    
    GPIO_InitStruct.Pin = BUTTON_0_PIN | BUTTON_1_PIN | BUTTON_2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUTTON_INT_GPIO, &GPIO_InitStruct);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    power_reset_timeout_counter();
    switch (GPIO_Pin) {
        case BUTTON_0_PIN: 
            wakeup_reason = WAKEUP_REASON_BUTTON;
            break;
        
        case BUTTON_1_PIN: 
            wakeup_reason = WAKEUP_REASON_BUTTON;
            break;
        
        case BUTTON_2_PIN: 
            wakeup_reason = WAKEUP_REASON_BUTTON;
            break;

        case NFC_INT_PIN:
            wakeup_reason = WAKEUP_REASON_NFC;
            break;
                
        case BMA_400_INT2_PIN: 
            wakeup_reason = WAKEUP_REASON_NFC;
            break;
    }
}