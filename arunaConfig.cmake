
add_library(aruna STATIC
    #src/blinky.cpp
    ${aruna_DIR}/src/log/log.cpp
    ${aruna_DIR}/src/comm/portable/posix/Serial.cpp
    ${aruna_DIR}/src/comm/comm.cpp
    #src/control.cpp
    #src/drivers/comm/UART.cpp
    #src/drivers/control/L293D.cpp
)
target_include_directories(aruna PUBLIC ${aruna_DIR}/include)
target_link_libraries(aruna pthread)

export(PACKAGE aruna)