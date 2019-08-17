set(COMPONENT_SRCS
        "src/blinky/blinky.cpp"
        "src/log/log.cpp"
        "src/comm/comm.cpp"
        "src/control/control.cpp"
        "src/comm/portable/esp32/UART.cpp"
        "src/control/portable/esp32/Pwm.cpp"
        "src/control/ActuatorSet.cpp"
        )
set(COMPONENT_ADD_INCLUDEDIRS
        "include"
        )
set(COMPONENT_REQUIRES "MPUdriver")

register_component()
