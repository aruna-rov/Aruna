#
# Main component makefile.
#

COMPONENT_SRCDIRS := src src/drivers/comm src/drivers/control
COMPONENT_SRCS := src/blinky.cpp src/comm.cpp src/control.cpp src/drivers/comm/UART.cpp src/drivers/control/Pwm.cpp src/drivers/control/DriverSet.cpp src/log.cpp
COMPONENT_ADD_INCLUDEDIRS = include