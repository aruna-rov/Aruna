#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

# specify cross compilers and tools
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/test/STM32F103C8Tx/STM32F103C8Tx_FLASH.ld)

#Uncomment for hardware floating point
#SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -D__FPU_PRESENT=1)

#Uncomment for software floating point
SET(FPU_FLAGS "-mfloat-abi=soft")

SET(COMMON_FLAGS
        "-mcpu=cortex-m3 ${FPU_FLAGS} -mthumb -mthumb-interwork -ffunction-sections -fdata-sections \
    -g -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs -Os -specs=rdimon.specs")

SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -std=c++14")
SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT} -lrdimon")

PROJECT(STM32F103C8Tx C CXX ASM)
set(CMAKE_CXX_STANDARD 14)

add_definitions(-DUSE_HAL_DRIVER -DSTM32F103xB)

file(GLOB_RECURSE SOURCES "test/STM32F103C8Tx/startup/*.*" "test/STM32F103C8Tx/Drivers/*.*" "test/STM32F103C8Tx/Src/*.*" "test/log/*.*")

get_filename_component(full_path_test_cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/STM32F103C8Tx/Src/syscalls.c ABSOLUTE)
list(REMOVE_ITEM SOURCES ${full_path_test_cpp})

include_directories(test/STM32F103C8Tx/Inc test/STM32F103C8Tx/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy test/STM32F103C8Tx/Drivers/STM32F1xx_HAL_Driver/Inc test/STM32F103C8Tx/Drivers/CMSIS/Device/ST/STM32F1xx/Include test/STM32F103C8Tx/Drivers/CMSIS/Include)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

add_library(FreeRTOS-POSIX STATIC
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c
        )

target_include_directories(FreeRTOS-POSIX PUBLIC
        FreeRTOS-POSIX/include
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/include
        FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/include/portable
        FreeRTOS-POSIX/include/private
        test/STM32F103C8Tx/Inc/FreeRTOS-POSIX)

file(GLOB_RECURSE FREE-RTOS_SOURCES "test/STM32F103C8Tx/Middlewares/Third_Party/FreeRTOS/*.*")

add_library(FreeRTOS STATIC
        ${FREE-RTOS_SOURCES}
        )

target_include_directories(FreeRTOS PUBLIC
        test/STM32F103C8Tx/Inc
        test/STM32F103C8Tx/Middlewares/Third_Party/FreeRTOS/Source/include
        test/STM32F103C8Tx/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
        test/STM32F103C8Tx/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
        )

#    standalone unit test
set(aruna_DIR .)
find_package(aruna)

target_link_libraries(FreeRTOS-POSIX FreeRTOS)
add_dependencies(FreeRTOS-POSIX FreeRTOS)

add_dependencies(${PROJECT_NAME}.elf aruna)
target_link_libraries(${PROJECT_NAME}.elf aruna)

add_dependencies(${PROJECT_NAME}.elf FreeRTOS)
target_link_libraries(${PROJECT_NAME}.elf FreeRTOS)

add_dependencies(aruna FreeRTOS-POSIX)
target_link_libraries(aruna FreeRTOS-POSIX)

add_subdirectory(test/Catch2)
#find_package(Catch2 REQUIRED)
target_link_libraries(${PROJECT_NAME}.elf Catch2::Catch2)
#
#add_dependencies(${PROJECT_NAME}.elf FreeRTOS-POSIX)
#target_link_libraries(${PROJECT_NAME}.elf FreeRTOS-POSIX)






