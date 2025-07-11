#Cmake 基础配置 Cmake Configure
set(CMAKE_SYSTEM_NAME Generic)  # 指定系统名称为通用
set(CMAKE_SYSTEM_PROCESSOR arm)  # 指定处理器为ARM架构
cmake_minimum_required(VERSION 3.14)  # 设置最低CMake版本要求为3.14

set(CMAKE_COLOR_DIAGNOSTICS ON) # 启用彩色诊断输出

# 指定交叉编译器和工具 Specify Cross-Compilers and Tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc) # 指定C编译器
set(CMAKE_CXX_COMPILER arm-none-eabi-g++) # 指定C++编译器
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc) # 指定汇编编译器
set(CMAKE_AR arm-none-eabi-ar) # 指定用于创建库的工具
set(CMAKE_OBJCOPY arm-none-eabi-objcopy) # 指定目标文件转换工具
set(CMAKE_OBJDUMP arm-none-eabi-objdump) # 指定目标文件反汇编工具
set(SIZE arm-none-eabi-size) # 指定用于获取文件大小的工具
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # 指定CMake的测试目标为静态库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 导出编译命令，用于代码分析

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)



#####################################
# 项目设置 Project Settings
#####################################
project(RM25-Engineer-Template
        VERSION 0.0.1
        DESCRIPTION "RoboMaster 25 Engineer Robot Firmware Template"
        LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# 为Targets文件夹指定变量名
# 后面如果使用别的开发板，在Targets文件夹中为其新建文件夹即可
set(TGTDIR "DM-MC02-keil" CACHE STRING "Target folder name in ./Targets")

#####################################
# ASCII Art
#####################################
message(STATUS "
|
|   ██░ ██ ▓█████  ██▓     ██▓     ▒█████        █     █░ ▒█████   ██▀███   ██▓    ▓█████▄    
|  ▓██░ ██▒▓█   ▀ ▓██▒    ▓██▒    ▒██▒  ██▒     ▓█░ █ ░█░▒██▒  ██▒▓██ ▒ ██▒▓██▒    ▒██▀ ██▌   
|  ▒██▀▀██░▒███   ▒██░    ▒██░    ▒██░  ██▒     ▒█░ █ ░█ ▒██░  ██▒▓██ ░▄█ ▒▒██░    ░██   █▌   
|  ░▓█ ░██ ▒▓█  ▄ ▒██░    ▒██░    ▒██   ██░     ░█░ █ ░█ ▒██   ██░▒██▀▀█▄  ▒██░    ░▓█▄   ▌   
|  ░▓█▒░██▓░▒████▒░██████▒░██████▒░ ████▓▒░     ░░██▒██▓ ░ ████▓▒░░██▓ ▒██▒░██████▒░▒████▓    
|   ▒ ░░▒░▒░░ ▒░ ░░ ▒░▓  ░░ ▒░▓  ░░ ▒░▒░▒░      ░ ▓░▒ ▒  ░ ▒░▒░▒░ ░ ▒▓ ░▒▓░░ ▒░▓  ░ ▒▒▓  ▒    
|   ▒ ░▒░ ░ ░ ░  ░░ ░ ▒  ░░ ░ ▒  ░  ░ ▒ ▒░        ▒ ░ ░    ░ ▒ ▒░   ░▒ ░ ▒░░ ░ ▒  ░ ░ ▒  ▒    
|   ░  ░░ ░   ░     ░ ░     ░ ░   ░ ░ ░ ▒         ░   ░  ░ ░ ░ ▒    ░░   ░   ░ ░    ░ ░  ░    
|   ░  ░  ░   ░  ░    ░  ░    ░  ░    ░ ░           ░        ░ ░     ░         ░  ░   ░       
|                                                                                   ░  
|")

#####################################
# 编译选项 Compiler Options
#####################################
# 根据编译类型选择优化选项
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast) # 最大化速度优化
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g) # 包含调试信息的速度优化
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os) # 尺寸优化
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-O0 -g) # 不优化 包含所有调试信息
endif ()

# 设置全局编译器标志
set(CPU -mcpu=cortex-m7) # 设置处理器为Cortex-M7
set(FPU -mfpu=fpv5-d16) # 设置浮点单元
set(FLOAT_ABI -mfloat-abi=hard) # 设置浮点ABI
set(MCU ${CPU} -mthumb ${FPU} ${FLOAT_ABI}) # 定义MCU标志
add_compile_options(${MCU} -Wall -Wno-packed-bitfield-compat -fdata-sections -ffunction-sections -O0) # 添加编译选项
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>) # 禁用C++的RTTI
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>) # 设置汇编的预处理

# 创建可执行文件
add_executable(FIRMWARE.elf) # 定义生成文件的名称为FIRMWARE.elf

# 先将target.cmake引入，因为里边定义了链接器的位置接下来要用到
include(Targets/${TGTDIR}/target.cmake)

# 设置目标链接选项
if (NOT LINKERSCRIPT)
    message(FATAL_ERROR "Please set linker script file directories in Targets/<CURRENT_TARGET>/target.cmake file!")
endif ()
target_link_options(FIRMWARE.elf PUBLIC ${MCU} -specs=nano.specs) # 连接选项，使用nano规格
target_link_options(FIRMWARE.elf PUBLIC -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map) # 使用节省内存的链接器选项
target_link_options(FIRMWARE.elf PUBLIC -T ${PROJECT_SOURCE_DIR}/${LINKERSCRIPT}) # 指定链接脚本


# add_compile_definitions(DLYB_MAX_SELECT=8 DLYB_MAX_UNIT=4)


#####################################
# 引入CMake项目模块
#####################################
include(Drivers/Drivers.cmake) # 引入驱动模块
include(Middlewares/Middlewares.cmake) # 引入中间件模块
include(Applications/Applications.cmake) # 引入应用模块

#####################################
# 将ELF文件转换为HEX和BIN格式
#####################################
set(HEX_FILE ${PROJECT_BINARY_DIR}/FIRMWARE.hex) # 指定HEX文件输出路径
set(BIN_FILE ${PROJECT_BINARY_DIR}/FIRMWARE.bin) # 指定BIN文件输出路径

add_custom_command(TARGET FIRMWARE.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:FIRMWARE.elf> ${HEX_FILE} # 将ELF转换为HEX
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:FIRMWARE.elf> ${BIN_FILE} # 将ELF转换为BIN
        COMMENT "Building ${HEX_FILE}\nBuilding ${BIN_FILE}") # 在构建日志中显示
