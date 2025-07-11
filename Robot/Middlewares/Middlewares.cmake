# Add FreeRTOS Kernel
add_subdirectory(Middlewares/FreeRTOS-Kernel)
target_link_libraries(FIRMWARE.elf freertos_kernel)
