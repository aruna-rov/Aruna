//
// Created by noeel on 14-1-19.
//

#ifndef ARUNA_COMDRIVER_H
#define ARUNA_COMDRIVER_H

#include <aruna/Com.h>
namespace aruna { namespace drivers { namespace com {
class ComDriver {
public:

    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @param  package_size: size of the package
     * @retval  `COM_OK` if all went well.
     *          `COM_ERR_HARDWARE` if the hardware failes
     */
    virtual Com::err_t transmit(uint8_t *package, uint8_t package_size) = 0;
    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @param  package_size: size of the package
     * @param  priority: (0 hightest to 2 lowest) only supported if link is realtime.
     * @retval  `COM_OK` if all went well.
     *          `COM_ERR_HARDWARE` if the hardware failes
     */
    virtual Com::err_t transmit(uint8_t *package, uint8_t package_size, unsigned short priority) {
        return transmit(package, package_size);
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
    virtual Com::link_t getLinkType() {
        return Com::link_t::NONE;
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
    virtual Com::err_t start(){
        return Com::err_t::OK;
    }

    /**
     * Stop the driver.
     * @return com_err, `COM_OK` if started succesfully, `COM_ERR_HARDWARE` or other hardware error on failure.
     */
    virtual Com::err_t stop(){
        return Com::err_t::OK;
    }
};

}}}
#endif //ARUNA_COMDRIVER_H