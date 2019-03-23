//
// Created by noeel on 14-1-19.
//

#ifndef ARUNA_COMDRIVER_H
#define ARUNA_COMDRIVER_H

#include "Com.h"

class ComDriver {
public:

    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @retval  `COM_OK` if all went well.
     *          `COM_ERR_HARDWARE` if the hardware failes
     */
    virtual com_err transmit(com_transmitpackage_t package) = 0;
//    TODO use com_bin as transmit parameter

    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @param  priority: (0 hightest to 2 lowest) only supported if link is realtime.
     * @retval  `COM_OK` if all went well.
     *          `COM_ERR_HARDWARE` if the hardware failes
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
     * @return unsigned int speed in Bytes per second.
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
     * @return com_link_t. COM_NONE if not connected.
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
     * is the external hardware found and operationable?
     * @return true/false
     */
    virtual bool isHardwareConnected() {
        return isEndpointConnected();
    }

    /**
     * start the driver.
     * @return com_err, `COM_OK` if started succesfully, `COM_ERR_HARDWARE` or other hardware error on failure.
     */
    virtual com_err start(){
        return COM_OK;
    }

    /**
     * Stop the driver.
     * @return com_err, `COM_OK` if started succesfully, `COM_ERR_HARDWARE` or other hardware error on failure.
     */
    virtual com_err stop(){
        return COM_OK;
    }
};


#endif //ARUNA_COMDRIVER_H
