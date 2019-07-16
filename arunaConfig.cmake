
add_library(aruna STATIC
    #src/blinky.cpp
    ${aruna_DIR}/src/comm.cpp
    ${aruna_DIR}/src/FreeRTOSKernel.cpp
    #src/control.cpp
    #src/drivers/comm/UART.cpp
    #src/drivers/control/L293D.cpp
)
target_include_directories(aruna PUBLIC ${aruna_DIR}/include)

set(FreeRTOS-Sim_DIR ${aruna_DIR}/libs)
find_package(FreeRTOS-Sim)
target_link_libraries(aruna FreeRTOS-Sim)
add_dependencies(aruna FreeRTOS-Sim)
export(PACKAGE aruna)