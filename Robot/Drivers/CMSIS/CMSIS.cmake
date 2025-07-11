include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32H7xx/Include)

# CMSIS DSP Library
add_subdirectory(Drivers/CMSIS/DSP)
target_link_libraries(FIRMWARE.elf CMSISDSP)