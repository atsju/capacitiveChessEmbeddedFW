cmake_minimum_required(VERSION 3.19)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/stm32-cmake/cmake/stm32_gcc.cmake)
set(STM32_CUBE_L4_PATH ${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeL4)

project(capacitiveChessboard C ASM)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 REQUIRED)

add_subdirectory(BSP)
add_subdirectory(Segger)

add_executable(capacitiveChessboardEmbeddedFW.elf main.c)
target_link_libraries(capacitiveChessboardEmbeddedFW.elf
    HAL::STM32::L4::RCC
    HAL::STM32::L4::GPIO
    HAL::STM32::L4::CORTEX
    HAL::STM32::L4::PWREx
    CMSIS::STM32::L4P5VG
    STM32::NoSys
    BSP
    sharpMemoryLCD
    buttons
    capacitiveDetection
    led
    SeggerRTT
)


#always display the code size
ADD_CUSTOM_TARGET(do_always ALL COMMAND arm-none-eabi-size --format=berkeley "capacitiveChessboardEmbeddedFW.elf"
                                DEPENDS capacitiveChessboardEmbeddedFW.elf)

