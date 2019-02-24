//
// Created by noeel on 14-1-19.
//

#ifndef ARUNA_COMDRIVER_H
#define ARUNA_COMDRIVER_H

#include <drivers/Com.h>

class ComDriver {
public:
    static const size_t nameSize = 7;

//    TODO documentatie
//    com_err registerd;

    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @retval
     */
    virtual com_err transmit(com_transmitpackage_t package) = 0;

    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @param  priority: only supported if link is realtime.
     * @retval
     */
    virtual com_err transmit(com_transmitpackage_t package, unsigned short priority) {
        return transmit(package);
    };

    /**
     * Get name of link.
     * @param buffer
     */
    void getName(char *buffer) {
        //    TODO testen
        *buffer = *getName();
    };

    /**
     * Get name of the driver
     * @return pointer to char array.
     */
    virtual char* getName() = 0;

    /**
     * Get speed of link (B/s)
     * @return
     */
    virtual unsigned int getSpeed() {
        return 0;
    }

    /**
     * Does the link have time guarantee?
     * @return true, is does, false it does not.
     */
    virtual bool isRealTime() {
        return false;
    }

    /**
     * Get link type (RADIO, WIRED, NONE)
     * @return
     */
    virtual com_link_t getLinkType() {
        return COM_NONE;
    };

    /**
     * Does the link have an active connection with a client?
     * @return true/false
     */
    virtual bool isEndpointConnected() {
        return false;
    }

    /**
     * is the hardware found and operationable?
     * @return true/false
     */
    virtual bool isHardwareConnected() {
        return isEndpointConnected();
    }

    /**
     * start the driver.
     * @return
     */
//     TODO documentatie
    virtual com_err start(){
        return COM_OK;
    }

    /**
     * Stop the driver.
     * @return
     */
    virtual com_err stop(){
        return COM_OK;
    }

    /**
     * Constuctor
     */
    ComDriver() {
        start();
    }

    /**
     * Destructor
     */
    ~ComDriver() {
        stop();
    }

};


#endif //ARUNA_COMDRIVER_H
