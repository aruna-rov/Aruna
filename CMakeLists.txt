set(COMPONENT_SRCS
        "src/blinky.cpp"
        "src/Com.cpp"
        "src/control.cpp"
        "src/drivers/com/UART.cpp"
        "src/drivers/control/L293D.cpp"
        )
set(COMPONENT_ADD_INCLUDEDIRS
        "include"
        )
set(COMPONENT_REQUIRES "MPUdriver")

register_component()
