# TODO this if else is really ugly, is there no better way?
if (DEFINED IDF_PATH)
    #    included as lib in ESP-IDF
    set(COMPONENT_SRCS
            "src/log/log.cpp"
            "src/comm/comm.cpp"
            "src/movement/movement.cpp"
            "src/driver/port/ESP32/ESP32_UART.cpp"
            "src/movement/ActuatorSet.cpp"
            "src/driver/Stepper.cpp"
            "src/driver/port/ESP32/ESP32_Stepper.cpp"
            "src/driver/Dshot.cpp"
            "src/sis/Performer.cpp"
            "src/sis/reporter.cpp"
            "src/sis/watcher.cpp"
            "src/sensor/Water.cpp"
            "src/driver/ADS101x.cpp"
            "src/driver/I2C_master.cpp"
            "src/driver/port/ESP32/ESP32_I2C_master.cpp"
            "src/driver/Pwm.cpp"
            "src/driver/PCA9685.cpp"
            "src/movement/Actuator.cpp"
            "src/driver/port/ESP32/ESP32_MCPwm.cpp"
            "src/driver/UART.cpp"
            "src/driver/port/ESP32/ESP32_RMT_Dshot.cpp"
            "src/driver/ADC.cpp"
            "src/sensor/Rain40x16.cpp"
            "src/comm/Link.cpp"
            )
    set(COMPONENT_ADD_INCLUDEDIRS
            "include"
            )
    set(COMPONENT_REQUIRES esp_adc_cal)
    register_component()
else ()
    #    standalone unit test
    cmake_minimum_required(VERSION 3.15)
    project(Aruna)
    set(CMAKE_CXX_STANDARD 20)

    add_executable(aruna-test
            test/test.cpp
            test/log/LogTest.cpp
            test/movement/ActuatorTest.cpp
            test/movement/ActuatorSetTest.cpp
            test/comm/LinkTest.cpp
            test/driver/cross_platform/FT_I2CTest.cpp
            test/driver/ADS101xTest.cpp
            test/driver/ADCTest.cpp)

    set(aruna_DIR .)
    find_package(aruna)
    find_package(GTest REQUIRED CONFIG)
    add_dependencies(aruna-test aruna GTest::gmock)
    target_link_libraries(aruna-test aruna GTest::gmock)


endif ()