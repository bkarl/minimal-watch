#pragma once

#include "stm32l0xx_hal.h"
#include "power_state.h"

#define BMA_400_INT1_PIN GPIO_PIN_9
#define BMA_400_INT2_PIN GPIO_PIN_15
#define BMA_400_INT_GPIO GPIOA

#define NFC_INT_PIN GPIO_PIN_9
#define NFC_INT_GPIO GPIOB

#define BUTTON_INT_GPIO GPIOB
#define BUTTON_0_PIN GPIO_PIN_6 //Top left
#define BUTTON_1_PIN GPIO_PIN_4 //Bottom left
#define BUTTON_2_PIN GPIO_PIN_3 // Bottom right

enum WAKEUP_REASON {
    WAKEUP_REASON_NONE,
    WAKEUP_REASON_NFC,
    WAKEUP_REASON_BUTTON,
    WAKEUP_REASON_CHECK_STEPS,
    WAKEUP_REASON_SHOW_SECONDS,
    WAKEUP_REASON_DOUBLE_TAP,
    WAKEUP_REASON_ALARM
};

void interrupts_init();
void interrupts_init_gpio();
void interrupt_set_display_mode();

extern volatile uint8_t wakeup_reason;
