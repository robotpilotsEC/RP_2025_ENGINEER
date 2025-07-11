# Add Include Directories
target_include_directories(FIRMWARE.elf PRIVATE Applications/01_Configuration/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/00_Algorithm/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/02_Interface/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/03_Device/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/04_Module/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/05_System/Include)

# Add Source Files
file(GLOB_RECURSE CONF_SRC Applications/01_Configuration/*)
file(GLOB_RECURSE ALGO_SRC Applications/00_Algorithm/*)
file(GLOB_RECURSE INF_SRC  Applications/02_Interface/*)
file(GLOB_RECURSE DEV_SRC  Applications/03_Device/*)
file(GLOB_RECURSE MOD_SRC  Applications/04_Module/*)
file(GLOB_RECURSE SYS_SRC  Applications/05_System/*)

target_sources(FIRMWARE.elf PRIVATE 
                ${CONF_SRC} ${ALGO_SRC} ${INF_SRC} ${DEV_SRC}
                ${MOD_SRC} ${SYS_SRC})
