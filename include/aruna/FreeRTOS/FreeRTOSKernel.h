
#pragma once
namespace aruna {
    /*
     * Start the FreeRTOS simulator, needed for aruna to work. Will block cpu so start in thread.
     */
    void* vStartFreeRTOSKernel(void* arg);
}