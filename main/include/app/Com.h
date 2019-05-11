//
// Created by noeel on 7-1-19.
//

#ifndef ARUNA_COM_H
#define ARUNA_COM_H

#include <string.h>
#include <esp_log.h>
#include "queue"
#include "set"
#include "tuple"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/queue.h>

static const size_t COM_DATA_SIZE = 32;

typedef char com_data_t[COM_DATA_SIZE];

// state the status of the executed function, > 0 means no success
enum com_err {
    COM_OK                  = 0x00,
    COM_FAIL                = 0x100,

//    COM running status
    COM_ERR_NOT_STOPPED     = 0x101,
    COM_ERR_NOT_STARTED     = 0x102,
    COM_ERR_NOT_PAUSED      = 0x103,

//    hardware
    COM_ERR_HARDWARE        = 0x110,
    COM_ERR_NO_CONNECTION   = 0x112,
    COM_ERR_NO_RESPONSE     = 0x113,
    COM_ERR_PROTOCOL        = 0x114,

//    config
    COM_ERR_BUFFER_OVERFLOW = 0x120,
    COM_ERR_INVALID_PARAMETERS = 0x121,
    COM_ERR_TASK_FAILED     = 0x122,

//    channel/driver registation
    COM_ERR_NO_CHANNEL      = 0x130,
    COM_ERR_CHANNEL_EXISTS  = 0x131,
    COM_ERR_NO_DRIVER       = 0x132,
    COM_ERR_DRIVER_EXISTS   = 0x133,
};

// kind of link, wired, wireless of non existing
enum com_link_t {
    COM_RADIO,
    COM_WIRED,
    COM_NONE
};

// status of the COM object
enum com_status {
    COM_RUNNING,
    COM_STOPPED,
    COM_PAUSED
};

typedef uint8_t com_port_t;

//binary object used for transfer. TODO should also consist of: size
typedef uint8_t com_bin_t[(sizeof(com_port_t) * 2) + sizeof(com_data_t)];

/**
 * transmit ready package.
 */
struct com_transmitpackage_t {
    /**
     * @brief  channel who is sending the data.
     */
    com_port_t from_port;

    /**
    * to whom to send it to.
    */
    com_port_t to_port;

    uint8_t n;
	uint8_t priority;

    /**
     * @brief  Data to be transmitted.
     */
//     TODO moet eigenlijk malloc gebruiken anders kan je nu in iemand anders z'n ram schrijven als je lenght groter dan COM_DATA_SIZE
    com_data_t data;

    /**
     * size of the data
     */
    size_t data_lenght;

    size_t size;

    /**
     * Get binary array of transmitpackage, for sending over a link.
     * com_bin_t arrangement: from_port, to_port, data
     * @param transp package to make a binary from.
     * @param bin com_bin_t to store the data to.
     */
    static void transmitpackage_to_binary(com_transmitpackage_t transp, com_bin_t &bin) {
        memcpy(&bin[0], &transp.from_port, (sizeof(transp.from_port)));
        memcpy(&bin[sizeof(transp.from_port)], &transp.to_port, sizeof(transp.to_port));
        memcpy(&bin[sizeof(transp.from_port) + sizeof(transp.to_port)], &transp.data, sizeof(transp.data));
    }

    /**
     * get tansmitpackage of binary array.
     * @param bin array that stores the info
     * @param transp transmitpackage that the data need to go to.
     * @param bin_length length of the binary array
     * @return true if succeeded, false if not (not yet implemented, will always return 1)
     */
    static bool binary_to_transmitpackage(com_bin_t bin, com_transmitpackage_t &transp, size_t bin_length) {
        if (bin_length < (sizeof(transp.from_port) + sizeof(transp.to_port)))
            return 0;
        size_t dataLength = bin_length - (sizeof(transp.from_port) + sizeof(transp.to_port));
        memcpy(&transp.from_port, &bin[0], (sizeof(transp.from_port)));
        memcpy(&transp.to_port, &bin[sizeof(transp.from_port)], (sizeof(transp.to_port)));
        memcpy(transp.data, &bin[sizeof(transp.from_port) + sizeof(transp.to_port)], dataLength);
        transp.data_lenght = dataLength;
        return 1;
    }
} __attribute__((packed));

/**
 * endpoint type of a com channel
 */
struct com_channel_t {
    /**
     * @brief port nr. of the endpoint
     */
    com_port_t port;

    /**
     * @brief priority 0-2
     * 0 highest (reserverd for SIS), 2 lowest
     */
    short priority;

    /**
     * @brief handeler to handle incomming connections
     * @note incomming data is not garanteed to be `COM_DATA_SIZE` in length. Could be smaller.
     * @param: `com_transmitpackage_t` incomming data
     */
//     TODO documentation
    QueueHandle_t *handeler;

    bool operator<(const com_channel_t &b) const {
        return this->port < b.port;
    }

    bool operator==(const uint8_t port) const {
        return this->port == port;
    }
};


struct com_ack_handel_t {
	void *_this;
	com_transmitpackage_t transmitpackage;
};

#include "drivers/com/ComDriver.h"

class ComDriver;

class Com {
    friend ComDriver;
public:
    static const short CHANNEL_BUFFER_SIZE = 32;

// control functions

    /**
     * Constructor
     */
    Com();

    /**
     * @brief  Start new communication. Using `COM_LINK_HARDWARE` to define hardware
     * @retval com_err
     *  * `COM_HARDWARE_ERROR` if the hardware fails.
     *  * `COM_OK` great success!
     */
    com_err start();

    /**
     * @brief  Start new communication. Using `COM_LINK_HARDWARE` to define hardware
     * @param driver force to use specific driver.
     * @retval com_err
     *  * `COM_HARDWARE_ERROR` if the hardware fails.
     *  * `COM_OK` great success!
     */
    com_err start(ComDriver *driver);

    /**
     * @brief  Stop the communication, free all queue's, channels and buffers
     * @retval com_err
     *  * `COM_HARDWARE_ERROR` if the hardware fails.
     *  * `COM_OK` great success!
     *  * `COM_ERR_NOT_STARTED` if the com was not started.
     */
    com_err stop();

    /**
     * @brief  Register a new communication endpoint
     * @param  channel: com_endpoint_t object
     * @retval com_err
     *  * `COM_ERR_INVALID_PARAMETERS` if parameters are invalid
     *  * `COM_OK` if it was succesfully added.
     *  * `COM_ERR_CHANNEL_EXISTS` if the channel already exists.
     *  * `COM_ERR_BUFFER_OVERFLOW` channel buffer overflow
     */
    com_err register_channel(com_channel_t *channel);

    /**
     * @brief  unregister an endpoint
     * @param  channel: endpoint to be removed
     * @retval com_err
     *  * `COM_OK` if it was succesfully removed.
     *  * `COM_ERR_NO_CHANNEL` ain't got no channel, ain't got no knife.
     */
    com_err unregister_channel(com_channel_t &channel);

    /**
     * @brief  Send data.
     * @note
     * @param  data: com_datapackage_t to be placed in the queue.
     * @retval com_err
     *  * `COM_ERR_INVALID_PARAMETERS` if parameters are invalid
     *  * `COM_OK` if it was succesfully send.
     *  * `COM_ERR_NO_CONNECTION` if there is no connection, (not implimented)
     *  * `COM_ERR_BUFFER_OVERFLOW` if the data was not added to the bugger due an overflow,
     *  * `COM_ERR_NO_CHANNEL` if the channel does'nt exist.
     */
    com_err send(com_channel_t *channel, com_port_t to_port, com_data_t data, size_t data_size);

    /**
     * pause all communication. buffers, channels and queue's will be saved
     * @return com_err
     *  * `COM_OK` if it was a success,
     *  * `COM_ERR_NOT_STARTED` if the com was not started.
     */
    com_err pause();

    /**
     * resume all communication.
     * @return com_err
     *  * `COM_OK` if it was a success,
     *  * `COM_ERR_NOT_STARTED` if the com was not started.
     *  * `COM_ERR_NOT_PAUSED` if it was not paused.
     */
    com_err resume();

    // getters and setters

    /**
     * @brief  Get the status of the com link
     * @retval 1 is connected, 0 if not.
     */
    bool is_connected();

    /**
     * Get the running status of com
     * @return `com_status` (RUNNING, PAUSED, STOPPED)
     */
    com_status get_status();

    /**
     * @brief  Get connection hardware type
     * @retval com_link_type
     */
    com_link_t get_link_type();

    /**
     * get the name of the driver.
     * @param buffer char* to put the name into
     */
    void getName(char *buffer);

    /**
     * get the name of the driver
     * @return char*, name of the driver.
     */
    char *getName();

    /**
     * @brief get all names of the channels currently registered
     * @param buffer to write the names into.
     * @return com_err
     *  * `COM_OK` if it was a success
     *  * `COM_ERR_BUFFER_OVERFLOW` if the suplied buffer is to small,
     *  * `COM_ERR_INVALID_PARAMETERS if the parameters are incorrect,
     */
    com_err get_channels(char *buffer);

    /**
     * @brief  Get the speed of the link to the other hosts in kB/s
     * @note   returns 0 if there is no connection.
     * @retval int, speed
     */
    unsigned int get_speed();

    /**
     * register a driver to be a com driver candidate.
     * @param driver ComDriver object
     * @return com_err
     * * `COM_ERR_DRIVER_EXISTS` if driver already exists.
     * * `COM_ERR_BUFFER_OVERFLOW` driver buffer overflow
     * * `COM_OK` all is well :).
     */
    com_err register_candidate_driver(ComDriver *driver);

    /**
     * unregister a driver to be a comdiver candidate
     * @param driver ComDriver to be deleted
     * @return com_err
     * * `COM_OK` great success!
     * * `COM_ERR_NO_DRIVER` driver does'nt exists.
     */
    com_err unregister_candidate_driver(ComDriver *driver);

    /**
     * get all the com driver candidates
     */
    void get_candidate_drivers(char *buffer[]);
//     TODO come here and make it!

    /**
     * @brief  Interrupt incomming connection handeler
     * @param package that needs to be handeled.
     * @retval com_err
     * `COM_ERR_NOT_STARTED` com is not started
     * `COM_OK` handeled
     * `COM_ERR_NO_CHANNEL` there is no channel to handle it
     */
    com_err incoming_connection(com_transmitpackage_t package);

protected:

    static const short TRANSMIT_QUEUE_BUFFER_SIZE = 16;

    /**
     * @brief set the com status
     * @param status: new status
     */
    void set_status(com_status status);

private:

    // variables

    static constexpr char *LOG_TAG = (char *) "COM";

    std::set<ComDriver *> driverCandidates;

    /**
     * transmission queue
     */
    QueueHandle_t transmission_queue[3];

    /**
     * xQueue set
     */
    QueueSetHandle_t transmission_queue_set;

    /**
     * @brief all endpoints
     */
    std::set<com_channel_t> channels;

    /**
     * @brief stores the com status
     */
    com_status status = COM_STOPPED;

    /**
     * stores the driver.
     */
    ComDriver *driver;

    /**
     * RTOS task handeler for the tranmissionqueue handeler task
     */
    TaskHandle_t transmissionQueueHandeler_task;

    /**
     * @brief  Tramsmission handeler. Do not call directly, blocks CPU.
     * @retval None
     */
    void transmissionQueueHandeler();

    /**
     * static wrapper for transmissionQueueHandeler so that RTOS can access it.
     * @param _this
     */
    static void transmissionQueueHandeler(void *_this);


    /**
     * get the driver
     * @return ComdDriver object
     */
    ComDriver *getDriver();

    /**
     * pick the best available driver
     * @return tuple    0: ComDriver best candidate object.
     *                  1: com_err.
     * 1: `COM_ERR_NO_DRIVER` if no driver can be found
     * 1: `COM_OK` if all is well
     */
    std::tuple<ComDriver *, com_err> pickDriver();

    /**
     * set the driver
     * @param driver to use.
     */
    void setDriver(ComDriver &driver);

    /**
     * rate the driver on speed, errors, active connection, realtime, connection type etc.
     * @param driver
     * @return rating of the driver. Higher is better.
     */
    unsigned int rateDriver(ComDriver &driver);

    /**
     * pick a new best driver, dont call directly will delete your process.
     */
    void _selectDriverTask();

    /**
     * static wrapper for _selectDriverTask so that it can be used in an RTOS task.
     * @param _this, that, there
     */
    static void _selectDriverTask(void *_this);

    /**
     * start a task to select a driver, does not block.
     */
    void selectDriverTask();


	void acknowledge_handler_task(com_transmitpackage_t transmitpackage_to_watch);
	static void _acknowledge_handler_task(void *handle);
};

extern Com COM;

#endif //ARUNA_COM_H
