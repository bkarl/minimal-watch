cmake_minimum_required(VERSION 3.13)
set(CMAKE_TOOLCHAIN_FILE /opt/stm32-cmake/cmake/stm32_gcc.cmake)
set(STM32_CUBE_L0_PATH /opt/cubemx/STM32CubeL0)

project(minimal-watch C ASM)

find_package(CMSIS COMPONENTS STM32L011 REQUIRED)
find_package(HAL COMPONENTS STM32L0 REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
 

set(PROJECT_SOURCES
    main.c
    display.c
    interrupt.c
    power_state.c
    rtc.c
    stm32l0xx_hal_msp.c
    stm32l0xx_it.c
)

add_executable(minimal-watch ${PROJECT_SOURCES})
link_directories(lib)

target_compile_options(minimal-watch PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(minimal-watch
    HAL::STM32::L0::RCC
    HAL::STM32::L0::RCCEx
    HAL::STM32::L0::GPIO
    HAL::STM32::L0::I2C
    HAL::STM32::L0::I2CEx
    HAL::STM32::L0::CORTEX
    HAL::STM32::L0::PWR
    HAL::STM32::L0::PWREx
    HAL::STM32::L0::RTC
    HAL::STM32::L0::TIM
    HAL::STM32::L0::TIMEx
    CMSIS::STM32::L011G4
    )
