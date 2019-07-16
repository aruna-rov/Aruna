add_library(FreeRTOS-Sim STATIC
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/croutine.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/event_groups.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/list.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/queue.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/tasks.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/timers.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/portable/MemMang/heap_3.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/portable/GCC/POSIX/port.c

    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/blocktim.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/countsem.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/GenQTest.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/QPeek.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/recmutex.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/BlockQ.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/death.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/dynamic.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/flop.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/integer.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/PollQ.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/semtest.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Full/print.c

    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/AbortDelay.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/EventGroupsDemo.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/IntSemTest.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/QueueSet.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/QueueSetPolling.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/QueueOverwrite.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/TaskNotify.c
    ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/Minimal/TimerDemo.c
)
target_include_directories(FreeRTOS-Sim PUBLIC
        ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/include
        ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Source/portable/GCC/POSIX
        ${FreeRTOS-Sim_DIR}/FreeRTOS-Sim/Demo/Common/include
        # for the FreeRTOSConfig.h
        ${aruna_DIR}/include/aruna/FreeRTOS
)
target_compile_definitions(FreeRTOS-Sim PRIVATE DEBUG=1 __GCC_POSIX__=1 MAX_NUMBER_OF_TASKS=300)
target_link_libraries(FreeRTOS-Sim pthread)