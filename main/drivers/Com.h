//
// Created by noeel on 7-1-19.
//

#ifndef ARUNA_COM_H
#define ARUNA_COM_H

#include "queue"
#include "set"
#include "tuple"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const size_t COM_DATA_SIZE = 32;
static const size_t COM_ENDPOINT_NAME_SIZE = 7;

typedef char com_data_t[COM_DATA_SIZE];

enum com_err {
    COM_OK,
    COM_FAIL,
    COM_ERR_NOT_STARTED,
    COM_ERR_NOT_PAUSED,
    COM_ERR_NOT_STOPPED,
    COM_ERR_NO_CONNECTION,
    COM_ERR_BUFFER_OVERFLOW,
    COM_ERR_INVALID_PARAMETERS,
    COM_ERR_HARDWARE,
    COM_ERR_PROTOCOL,
    COM_ERR_NO_RESPONSE,
    COM_ERR_NO_CHANNEL,
    COM_ERR_CHANNEL_EXISTS,
    COM_ERR_DRIVER_EXISTS,
    COM_ERR_NO_DRIVER,
    COM_ERR_TASK_FAILED
};

enum com_link_t {
    COM_RADIO,
    COM_WIRED,
    COM_NONE
};

enum com_status {
    COM_RUNNING,
    COM_STOPPED,
    COM_PAUSED
};

typedef uint8_t com_port_t;
typedef char com_bin_t[(sizeof(com_port_t) * 2) + sizeof(com_data_t)];

/**
 * transmit ready package.
 */
struct com_transmitpackage_t {
    /**
     * @brief  endpoint who is sending the data.
     */
    com_port_t from_port;

    /**
    * to whom to send it to.
    */
    com_port_t to_port;

    /**
     * @brief  Data to be transmitted.
     */
    com_data_t data;

    /*
     * Get binary array of transmitpackage.
     * arrangement: from_port, to_port, data.
     * @return pointer to char array.
     */
//    TODO documentatie bijwerken
    static void transmitpackage_to_binary(com_transmitpackage_t transp, com_bin_t *bin) {
//        TODO data word nu gelinked, is beter als het gekopieerd word.
        *bin[0] = transp.from_port;
        *bin[sizeof(from_port)] = transp.to_port;
        *bin[sizeof(to_port) + sizeof(from_port)] = *transp.data;
    }

//  TODO documentatie
    static void binary_to_transmitpackage(com_bin_t bin, com_transmitpackage_t *transp) {
        //        TODO data word nu gelinked, is beter als het gekopieerd word.
        transp->from_port = (com_port_t) bin[0];
        transp->to_port = (com_port_t) bin[sizeof(from_port)];
        *transp->data = bin[sizeof(to_port) + sizeof(from_port)];
    }
};

/**
 * endpoint type of a com channel
 */
struct com_endpoint_t {
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
     * @param: `com_data_t` incomming data
     */
//     TODO nu heeft de handeler alleen de data, het is ook handig als hij de afzender weet.
    void (*handeler)(com_data_t);

    bool operator<(const com_endpoint_t &b) const {
        return this->port < b.port;
    }

    bool operator==(const uint8_t port) const {
        return this->port == port;
    }
};

/**
 * datapackage containing the endpoint and data to be send.
 */
// TODO deze naam lijkt te erg op `com_data_t`. Moet ff allemaal wat duidelijk.
struct com_datapackage_t {
    /**
     * @brief  Communication endpoint to send reply to.
     * @retval None
     */
    struct com_endpoint_t *com_endpoint;

    /**
    * to whom to send it to.
    */
    com_port_t to_port;

    /**
     * @brief Data to be send.
     */
    com_data_t *data;
};

#include "com/ComDriver.h"

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
//     TODO documentatie
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
     * @param  endpoint: com_endpoint_t object
     * @retval com_err
     *  * `COM_ERR_INVALID_PARAMETERS` if parameters are invalid
     *  * `COM_OK` if it was succesfully added.
     */
    com_err register_channel(com_endpoint_t *endpoint);

    /**
     * @brief  unregister an endpoint
     * @param  endpoint: endpoint to be removed
     * @retval com_err
     *  * `COM_ERR_INVALID_PARAMETERS` if parameters are invalid
     *  * `COM_OK` if it was succesfully removed.
     */
    com_err unregister_channel(com_endpoint_t &endpoint);

    /**
     * @brief  Send data.
     * @note
     * @param  data: com_datapackage_t to be placed in the queue.
     * @retval com_err
     *  * `COM_ERR_INVALID_PARAMETERS` if parameters are invalid
     *  * `COM_OK` if it was succesfully send.
     *  * `COM_ERR_NO_CONNECTION` if there is no connection,
     *  * `COM_ERR_BUFFER_OVERFLOW` if the data was not added to the bugger due an overflow,
     */
    com_err send(com_datapackage_t data);

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

//    TODO documentatie
    void getName(char *buffer);

//    TODO documentatie en naam veranderen
    char *getName();


    /**
     * @brief get all names of the channels currently registered
     * @param buffer to write the names into.
     * @return com_err
     *  * `COM_OK` if it was a success
     *  * `COM_ERR_BUFFER_OVERFLOW` if the suplied buffer is to small,
     *  * `COM_ERR_INVALID_PARAMETERS if the parameters are incorrect,
     */
    com_err get_channels(char buffer[COM_ENDPOINT_NAME_SIZE][CHANNEL_BUFFER_SIZE]);

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
     */
//     TODO documenatie: com_err
    com_err register_candidate_driver(ComDriver *driver);

    /**
     * unregister a driver to be a comdiver candidate
     * @param driver ComDriver to be deleted
     * @return com_err
     */
//     TODO documenatie com_err
    com_err unregister_candidate_driver(ComDriver *driver);

    /**
     * get all the com driver candidates
     */
//     TODO maken, en documenatie
    void get_candidate_drivers(char *buffer[]);

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
     * @brief high priority transmission queue
     */
    std::queue<com_transmitpackage_t> transmission0_queue[TRANSMIT_QUEUE_BUFFER_SIZE];

    /**
     * @brief mid priority transmission queue
     */
    std::queue<com_transmitpackage_t> transmission1_queue[TRANSMIT_QUEUE_BUFFER_SIZE];

    /**
     * @brief low priority transmission queue
     */
    std::queue<com_transmitpackage_t> transmission2_queue[TRANSMIT_QUEUE_BUFFER_SIZE];

    /**
     * @brief incomming stransmission queue.
     */
//     TODO is deze nog wel nodig?
    std::queue<com_transmitpackage_t> incomming_transmission_queue[TRANSMIT_QUEUE_BUFFER_SIZE];

    /**
     * @brief all endpoints
     */
    std::set<com_endpoint_t> channels;

    /**
     * @brief stores the com status
     */
    com_status status = COM_STOPPED;

    /**
     * stores the driver.
     */
    ComDriver *driver;

//    TODO documentatie
    TaskHandle_t transmissionQueueHandeler_task;

    /**
     * @brief  Strips `com_datapackage_t` to `_com_transmitpackage_t`
     * @param  datap: paclage to be stripped.
     * @retval _com_transmitpackage_t
     */
    com_transmitpackage_t datapackage2transmitpackage(com_datapackage_t datap);

    /**
     * @brief  Tramsmission handeler. Do not call directly, blocks CPU.
     * @retval None
     */
    void transmissionQueueHandeler();

//    TODO documentation
    static void transmissionQueueHandeler(void *_this);


//    TODO misschien moeten de driver public gemaakt worden?
    /**
     * get the driver
     * @return ComdDriver object
     */
    ComDriver *getDriver();

    /**
     * pick the best available driver
     * @return ComDriver best candidate object.
     */
//     TODO documentatie
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
     * @brief  Interrupt incomming connection handeler
     * @param package
     * @retval None
     */
//     TODO documentatie
    com_err incoming_connection(com_transmitpackage_t package);

//    TODO documentatie
    void _selectDriverTask();

    static void _selectDriverTask(void *_this);

    void selectDriverTask();


};

extern Com COM;

#endif //ARUNA_COM_H
