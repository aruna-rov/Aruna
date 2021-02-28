# TODO set sources in one file only. Now you need to updat CMakelists.txt and arunaConfig.cmake if sources change.
set(aruna_sources
        "${aruna_DIR}/src/log/log.cpp"
        "${aruna_DIR}/src/comm/comm.cpp"
        "${aruna_DIR}/src/movement/movement.cpp"
        "${aruna_DIR}/src/movement/ActuatorSet.cpp"
#        "${aruna_DIR}/src/driver/Stepper.cpp"
        "${aruna_DIR}/src/driver/Dshot.cpp"
        "${aruna_DIR}/src/sis/Performer.cpp"
        "${aruna_DIR}/src/sis/reporter.cpp"
        "${aruna_DIR}/src/sis/watcher.cpp"
        "${aruna_DIR}/src/sensor/Water.cpp"
        "${aruna_DIR}/src/driver/ADS101x.cpp"
        "${aruna_DIR}/src/driver/I2C_master.cpp"
        "${aruna_DIR}/src/driver/Pwm.cpp"
        "${aruna_DIR}/src/driver/PCA9685.cpp"
        "${aruna_DIR}/src/movement/Actuator.cpp"
        "${aruna_DIR}/src/driver/UART.cpp"
        "${aruna_DIR}/src/driver/ADC.cpp"
        "${aruna_DIR}/src/sensor/Rain40x16.cpp"
        "${aruna_DIR}/src/comm/Link.cpp"
        )

if (UNIX)
    set(aruna_sources
            ${aruna_sources}
            ${aruna_DIR}/src/driver/port/POSIX/POSIX_UART.cpp
            )

endif()

add_library(aruna STATIC
        ${aruna_sources}
        )
target_include_directories(aruna PUBLIC ${aruna_DIR}/include)

if (UNIX)
    target_link_libraries(aruna pthread)
endif()
export(PACKAGE aruna)