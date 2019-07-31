set(COMPONENT_SRCS
        "src/blinky.cpp"
        "src/log.cpp"
        "src/comm.cpp"
        "src/control.cpp"
        "src/drivers/comm/UART.cpp"
        "src/drivers/control/Pwm.cpp"
        )
set(COMPONENT_ADD_INCLUDEDIRS
        "include"
        )
set(COMPONENT_REQUIRES "MPUdriver")

register_component()
