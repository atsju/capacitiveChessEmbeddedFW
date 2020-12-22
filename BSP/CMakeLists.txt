set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

add_library(BSP INTERFACE)
target_include_directories(BSP INTERFACE .)

add_library(font INTERFACE)
target_sources(font INTERFACE ../STM32CubeL4/Utilities/Fonts/font16.c)
target_include_directories(font INTERFACE ../STM32CubeL4/Utilities/Fonts)

add_library(sharpMemoryLCD INTERFACE)
target_sources(sharpMemoryLCD INTERFACE sharpMemoryLCD/LS013B7DH03.c)
target_include_directories(sharpMemoryLCD INTERFACE sharpMemoryLCD)
target_link_libraries(sharpMemoryLCD INTERFACE
    HAL::STM32::L4::GPIO
    HAL::STM32::L4::TIM
    HAL::STM32::L4::SPI
    font)
