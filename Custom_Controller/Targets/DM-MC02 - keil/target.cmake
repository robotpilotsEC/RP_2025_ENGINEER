cmake_minimum_required (VERSION 3.14)

# Add Definitions
add_definitions(-DUSE_HAL_DRIVER -DSTM32H723xx)

# Add Include Directories
include_directories(Targets/${TGTDIR}/BSP
                    Targets/${TGTDIR}/Core/Inc)

# Add Sources Files
file(GLOB BSPSRC  Targets/${TGTDIR}/BSP/*.*)
file(GLOB CORESRC Targets/${TGTDIR}/Core/Src/*.*)
file(GLOB STARTUP Targets/${TGTDIR}/Core/Startup/*.s)

target_sources(FIRMWARE.elf PRIVATE ${BSPSRC})
target_sources(FIRMWARE.elf PRIVATE ${CORESRC})
target_sources(FIRMWARE.elf PRIVATE ${STARTUP})

# Set Linker Script
set(LINKERSCRIPT Targets/${TGTDIR}/STM32H723VGTX_FLASH.ld CACHE STRING "Set Linker Script File")

# Set FreeRTOS Config File
set(FREERTOS_CONFIG_FILE_DIRECTORY ${PROJECT_SOURCE_DIR}/Targets/${TGTDIR}/BSP CACHE STRING "Set FreeRTOS Config File")
set(FREERTOS_PORT GCC_ARM_CM7 CACHE STRING "Set FreeRTOS Port Target")
