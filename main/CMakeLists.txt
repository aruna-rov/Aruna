set(COMPONENT_SRCS "main.cpp" "app/Com.cpp" "drivers/com/UART.cpp" "app/blinky.cpp" "drivers/control/control.cpp" "drivers/control/L293D.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include ../libs/MPUdriver/include ../libs/I2Cbus/include")

register_component()