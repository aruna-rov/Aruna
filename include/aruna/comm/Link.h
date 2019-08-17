//
// Created by noeel on 14-1-19.
//

#ifndef ARUNA_COMMLINK_H
#define ARUNA_COMMLINK_H

#include "aruna/arunaTypes.h"
#include <aruna/comm/commTypes.h>
namespace aruna { namespace comm {
class Link {
public:

    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @param  package_size: size of the package
     * @retval  `OK` if all went well.
     *          `HARDWARE` if the hardware failes
     */
    virtual err_t transmit(uint8_t *package, uint8_t package_size) = 0;

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
     * @return link_t. NONE if not connected.
     */
    virtual link_t getLinkType() {
        return link_t::NONE;
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
     * @return err_t, `OK` if started succesfully, `HARDWARE` or other hardware error on failure.
     */
    virtual err_t start(){
        return err_t::OK;
    }

    /**
     * Stop the driver.
     * @return err_t, `OK` if started succesfully, `HARDWARE` or other hardware error on failure.
     */
    virtual err_t stop(){
        return err_t::OK;
    }
};

}}
#endif //ARUNA_COMMLINK_H
