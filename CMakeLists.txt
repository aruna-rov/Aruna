# TODO this if else is really ugly, is there no better way?
if (DEFINED IDF_PATH)
#    included as lib in ESP-IDF
set(COMPONENT_SRCS
        "src/blinky/blinky.cpp"
        "src/log/log.cpp"
        "src/comm/comm.cpp"
        "src/control/control.cpp"
        "src/driver/port/ESP32/ESP32_UART.cpp"
        "src/control/ActuatorSet.cpp"
        "src/driver/Stepper.cpp"
        "src/driver/port/ESP32/ESP32_Stepper.cpp"
        "src/driver/Dshot.cpp"
        "src/sis/Performer.cpp"
        "src/sis/reporter.cpp"
        "src/sis/watcher.cpp"
        "src/sis/Water.cpp"
        "src/sis/portable/esp32/WaterSensor.cpp"
        "src/driver/ADS101x/ADS101x.cpp"
        "src/driver/ADS101x/portable/ESP32/ADS101x_ESP32.cpp"
        "src/sis/ADS101xWaterSensor.cpp"
        "src/driver/I2C_master.cpp"
        "src/driver/port/ESP32/ESP32_I2C_master.cpp"
        "src/driver/Pwm.cpp"
        "src/driver/PCA9685.cpp"
        "src/control/Actuator.cpp"
        "src/driver/port/ESP32/ESP32_MCPwm.cpp"
        "src/driver/UART.cpp"
        "src/driver/port/ESP32/ESP32_RMT_Dshot.cpp"
        )
set(COMPONENT_ADD_INCLUDEDIRS
        "include"
        )
set(COMPONENT_REQUIRES esp_adc_cal)
register_component()
else()
#    standalone unit test
    cmake_minimum_required(VERSION 3.15)
    project(Aruna)
    set(CMAKE_CXX_STANDARD 14)
    file(GLOB_RECURSE ARUNA_TEST_SOURCES
            "test/*.*"
            )

    add_executable(aruna-test ${ARUNA_TEST_SOURCES})
    set(aruna_DIR .)
    find_package(aruna)
    add_dependencies(aruna-test aruna)
    target_link_libraries(aruna-test aruna)
endif()