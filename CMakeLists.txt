# TODO this if else is really ugly, is there no better way?
if (DEFINED IDF_PATH)
#    included as lib in ESP-IDF
set(COMPONENT_SRCS
        "src/blinky/blinky.cpp"
        "src/log/log.cpp"
        "src/comm/comm.cpp"
        "src/control/control.cpp"
        "src/comm/portable/esp32/UART.cpp"
        "src/control/portable/esp32/Pwm.cpp"
        "src/control/ActuatorSet.cpp"
        "src/control/Stepper.cpp"
        "src/control/portable/esp32/Stepper.cpp"
        "src/control/portable/esp32/Dshot.cpp"
        )
set(COMPONENT_ADD_INCLUDEDIRS
        "include"
        )
set(COMPONENT_REQUIRES "MPUdriver")

register_component()
else()
#    standalone unit test
    cmake_minimum_required(VERSION 3.15)
    project(Aruna)
    file(GLOB_RECURSE ARUNA_TEST_SOURCES
            "test/*.*"
            )

    add_executable(aruna-test ${ARUNA_TEST_SOURCES})
    set(aruna_DIR .)
    find_package(aruna)
    add_dependencies(aruna-test aruna)
    target_link_libraries(aruna-test aruna)
endif()