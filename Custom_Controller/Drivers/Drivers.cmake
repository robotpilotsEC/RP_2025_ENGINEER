# CMSIS Library
include(Drivers/CMSIS/CMSIS.cmake)

# STM32H7xx hal硬件抽象库
add_subdirectory(Drivers/stm32h7xx-hal-driver)
target_link_libraries(FIRMWARE.elf stm32h7xx_hal_driver)

# BMI 08x Sensor API Library
add_subdirectory(Drivers/BMI08x_SensorAPI)
target_link_libraries(FIRMWARE.elf BMI08x_SensorAPI)
