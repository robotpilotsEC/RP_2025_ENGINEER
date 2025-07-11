# Add Include Directories
target_include_directories(FIRMWARE.elf PRIVATE Applications/Configuration/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/Algorithm/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/Interface/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/Device/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/Module/Include)
target_include_directories(FIRMWARE.elf PRIVATE Applications/System/Include)

# Add Source Files
file(GLOB_RECURSE CONF_SRC Applications/Configuration/*)
file(GLOB_RECURSE ALGO_SRC Applications/Algorithm/*)
file(GLOB_RECURSE INF_SRC  Applications/Interface/*)
file(GLOB_RECURSE DEV_SRC  Applications/Device/*)
file(GLOB_RECURSE MOD_SRC  Applications/Module/*)
file(GLOB_RECURSE SYS_SRC  Applications/System/*)

target_sources(FIRMWARE.elf PRIVATE 
                ${CONF_SRC} ${ALGO_SRC} ${INF_SRC} ${DEV_SRC}
                ${MOD_SRC} ${SYS_SRC})
