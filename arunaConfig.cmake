# TODO set sources in one file only. Now you need to updat CMakelists.txt and arunaConfig.cmake if sources change.
set(aruna_sources
        "${aruna_DIR}/src/log/log.cpp"
        "${aruna_DIR}/src/comm/comm.cpp"
        "${aruna_DIR}/src/movement/movement.cpp"
        "${aruna_DIR}/src/movement/ActuatorSet.cpp"
        #                "${aruna_DIR}/src/driver/Stepper.cpp"
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

add_library(aruna)

if (UNIX)
    set(aruna_sources
            ${aruna_sources}
            ${aruna_DIR}/src/driver/port/POSIX/POSIX_UART.cpp
            )
    target_link_libraries(aruna pthread)

endif ()


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
# Find libftdi1
find_package(libftdi1)

# Find libmpsse
find_package(libmpsse)

if ( (${LIBFTDI1_FOUND}) AND (${LIBMPSSE_FOUND}) )
    message(STATUS "Enabling FTDI and MPSSE support")
    set(ENABLE_FTDI 1)
    add_definitions(-DARUNA_ENABLE_FTDI)
    target_link_libraries(aruna ${LIBFTDI1_LIBRARIES} ${LIBMPSSE_LIBRARIES})
    target_include_directories(aruna PUBLIC ${LIBFTDI1_INCLUDE_DIRS} ${LIBMPSSE_INCLUDE_DIRS})
    set(aruna_sources
            ${aruna_sources}
            "${aruna_DIR}/src/driver/port/cross_platform/FT_I2C.cpp"
            )
else()
    message(STATUS "Disabling FTDI code due to missing libftdi1 or libmpsse:")
    message(STATUS "  LIBFTDI1_FOUND = ${LIBFTDI1_FOUND}")
    message(STATUS "  LIBMPSSE_FOUND = ${LIBMPSSE_FOUND}")
endif()


target_sources(aruna PUBLIC
        ${aruna_sources}
        )
target_include_directories(aruna PUBLIC ${aruna_DIR}/include)

export(PACKAGE aruna)