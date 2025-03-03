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
    bma400.c
    power_state.c
    i2c_bitbanging.c
    rtc.c
    nfc.c
    stm32l0xx_hal_msp.c
    stm32l0xx_it.c
)

add_executable(minimal-watch ${PROJECT_SOURCES})
target_link_options(minimal-watch
  PUBLIC
    LINKER:-Map=foo.map
)
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
    HAL::STM32::L0::RTCEx
    HAL::STM32::L0::TIM
    HAL::STM32::L0::TIMEx
    CMSIS::STM32::L011G4
    )

  add_custom_command (TARGET minimal-watch POST_BUILD COMMAND arm-none-eabi-nm -S --size-sort -s minimal-watch.elf)
  add_custom_command (TARGET minimal-watch POST_BUILD COMMAND arm-none-eabi-size -t minimal-watch.elf)
