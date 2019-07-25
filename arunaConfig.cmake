
add_library(aruna STATIC
    #src/blinky.cpp
    ${aruna_DIR}/src/log.cpp
    ${aruna_DIR}/src/drivers/comm/SerialPosix.cpp
    ${aruna_DIR}/src/comm.cpp
    #src/control.cpp
    #src/drivers/comm/UART.cpp
    #src/drivers/control/L293D.cpp
)
target_include_directories(aruna PUBLIC ${aruna_DIR}/include)

export(PACKAGE aruna)