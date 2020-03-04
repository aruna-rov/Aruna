set(aruna_sources
        ${aruna_DIR}/src/log/log.cpp
        ${aruna_DIR}/src/comm/comm.cpp
        )

if (UNIX)
    set(aruna_sources
            ${aruna_sources}
            ${aruna_DIR}/src/comm/portable/posix/Serial.cpp
            )
    target_link_libraries(aruna pthread)

endif()

add_library(aruna STATIC
        ${aruna_sources}
        )
target_include_directories(aruna PUBLIC ${aruna_DIR}/include)

export(PACKAGE aruna)