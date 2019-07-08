//
// Created by noeel on 14-1-19.
//

#ifndef ARUNA_COMMDRIVER_H
#define ARUNA_COMMDRIVER_H

#include <aruna/comm.h>
namespace aruna { namespace drivers { namespace comm {
class CommDriver {
public:

    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @param  package_size: size of the package
     * @retval  `OK` if all went well.
     *          `HARDWARE` if the hardware failes
     */
    virtual aruna::comm::err_t transmit(uint8_t *package, uint8_t package_size) = 0;
    /**
     * @brief  directly transmit a package on the link.
     * @param  package: package to be send
     * @param  package_size: size of the package
     * @param  priority: (0 hightest to 2 lowest) only supported if link is realtime.
     * @retval  `OK` if all went well.
     *          `HARDWARE` if the hardware failes
     */
    virtual aruna::comm::err_t transmit(uint8_t *package, uint8_t package_size, unsigned short priority) {
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
     * @return link_t. NONE if not connected.
     */
    virtual aruna::comm::link_t getLinkType() {
        return aruna::comm::link_t::NONE;
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
    virtual aruna::comm::err_t start(){
        return aruna::comm::err_t::OK;
    }

    /**
     * Stop the driver.
     * @return err_t, `OK` if started succesfully, `HARDWARE` or other hardware error on failure.
     */
    virtual aruna::comm::err_t stop(){
        return aruna::comm::err_t::OK;
    }
};

}}}
#endif //ARUNA_COMMDRIVER_H
