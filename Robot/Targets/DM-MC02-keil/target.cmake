cmake_minimum_required (VERSION 3.14)

# Add Definitions
add_definitions(-DUSE_HAL_DRIVER -DSTM32H723xx)

# Add Include Directories
include_directories(Targets/${TGTDIR}/BSP
                    Targets/${TGTDIR}/Core/Inc
                    Targets/${TGTDIR}/USB_DEVICE/App
                    Targets/${TGTDIR}/USB_DEVICE/Target
                    Targets/${TGTDIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
                    Targets/${TGTDIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc)

# Add Sources Files
file(GLOB BSPSRC  Targets/${TGTDIR}/BSP/*.*)
file(GLOB CORESRC Targets/${TGTDIR}/Core/Src/*.*)
file(GLOB STARTUP Targets/${TGTDIR}/Core/Startup/*.s)
file(GLOB_RECURSE USBSRC Targets/${TGTDIR}/USB_DEVICE/*.*)
file(GLOB_RECURSE MiddlewaresRC Targets/${TGTDIR}/Middlewares/*.*)


target_sources(FIRMWARE.elf PRIVATE ${BSPSRC})
target_sources(FIRMWARE.elf PRIVATE ${CORESRC})
target_sources(FIRMWARE.elf PRIVATE ${STARTUP})
target_sources(FIRMWARE.elf PRIVATE ${USBSRC})
target_sources(FIRMWARE.elf PRIVATE ${MiddlewaresRC})

# Set Linker Script
set(LINKERSCRIPT Targets/${TGTDIR}/STM32H723VGTX_FLASH.ld CACHE STRING "Set Linker Script File")

# Set FreeRTOS Config File
set(FREERTOS_CONFIG_FILE_DIRECTORY ${PROJECT_SOURCE_DIR}/Targets/${TGTDIR}/BSP CACHE STRING "Set FreeRTOS Config File")
set(FREERTOS_PORT GCC_ARM_CM7 CACHE STRING "Set FreeRTOS Port Target")
