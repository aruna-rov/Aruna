set(aruna_sources
        ${aruna_DIR}/src/log/log.cpp
        ${aruna_DIR}/src/comm/comm.cpp
        ${aruna_DIR}/src/sis/watcher.cpp
        ${aruna_DIR}/src/sis/Performer.cpp
        ${aruna_DIR}/src/sis/Water.cpp
        ${aruna_DIR}/src/sis/reporter.cpp
        )

if (UNIX)
    set(aruna_sources
            ${aruna_sources}
            ${aruna_DIR}/src/comm/portable/posix/Serial.cpp
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