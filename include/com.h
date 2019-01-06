#ifndef ARUNA_COM_H
#define ARUNA_COM_H

#define COM_DATA_SIZE 32
#define COM_LINK_HARDWARE 27MHZ_RADIO
#define COM_BUFFER_SIZE 16
#define COM_ENDPOINT_NAME_SIZE 7

typedef enum {
    COM_OK,
    COM_ERR_NO_CONNECTION,
    COM_ERR_BUFFER_OVERFLOW,
    COM_ERR_INVALID_PARAMETERS,
    COM_HARDWARE_ERROR
} com_err;

typedef enum {
    RADIO,
    WIRED,
    NONE
} com_link_type;

struct com_endpoint_t;

typedef char com_data_t [COM_DATA_SIZE];

/**
 * transmit ready package.
 */
typedef struct {
    /**
     * @brief  endpoint who is sending the data.
     */
    char endpoint_name[COM_ENDPOINT_NAME_SIZE];

    /**
     * @brief  Data to be transmitted.
     */
    com_data_t data;
} _com_transmitpackage_t;

/**
 * datapackage containing the endpoint and data to be send.
 */
typedef struct {
    /**
     * @brief  Communication endpoint to send reply to.   
     * @retval None
     */
    struct com_endpoint_t *com_endpoint;

    /**
     * @brief Data to be send.
     */
    com_data_t data;
} com_datapackage_t;

/**
 * endpoint type of a com channel
 */
typedef struct {
    /**
     * @brief Name of the endpoint
     */
    char name[COM_ENDPOINT_NAME_SIZE];

    /**
     * @brief priority 0-2
     * 0 highest (reserverd for SIS), 2 lowest
     */
    int priority;

    /**
     * @brief handeler to handle incomming connections 
     * @note incomming data is not garanteed to be `COM_DATA_SIZE` in length. Could be smaller.
     * @param: `com_data_t` incomming data
     * @retval com_datapackage_t: reply to send back.
     */
    com_datapackage_t (*handeler)(com_data_t);
    // TODO infestigate if com_datapackage_t and com_endpoint_t dont loop.
} com_endpoint_t;

/**
 * @brief high priority transmission queue
 */
_com_transmitpackage_t _com_transmission0_queue[COM_BUFFER_SIZE];

/**
 * @brief mid priority transmission queue
 */
_com_transmitpackage_t _com_transmission1_queue[COM_BUFFER_SIZE];

/**
 * @brief low priority transmission queue
 */
_com_transmitpackage_t _com_transmission2_queue[COM_BUFFER_SIZE];

/**
 * @brief  Get the link type
 * @retval com_link_type
 */
com_link_type com_get_link_type();

/**
 * @brief  Get the speed of the link to the other hosts in kB/s
 * @note   returns 0 if there is no connection.
 * @retval int, speed
 */
unsigned int com_get_speed();

/**
 * @brief  Register a new communication endpoint
 * @param  endpoint: com_endpoint_t object
 * @retval com_err
 *  * `COM_ERR_INVALID_PARAMETERS` if parameters are invalid
 *  * `COM_OK` if it was succesfully added.
 */
com_err com_register_endpoint(com_endpoint_t endpoint);

/**
 * @brief  unregester an endpoint
 * @param  endpoint: endpoint to be removed
 * @retval com_err
 *  * `COM_ERR_INVALID_PARAMETERS` if parameters are invalid
 *  * `COM_OK` if it was succesfully removed.
 */
com_err com_unregister_endpoint(com_endpoint_t endpoint);

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
com_err com_send(com_datapackage_t data);

/**
 * @brief  Start communication. Using `COM_LINK_HARDWARE` to define hardware
 * @retval com_err
 *  * `COM_HARDWARE_ERROR` if the hardware fails.
 *  * `COM_OK` great success!
 */
com_err com_start();

/**
 * @brief  Stop the communication
 * @retval com_err
 *  * `COM_HARDWARE_ERROR` if the hardware fails.
 *  * `COM_OK` great success!
 */
com_err com_stop();

/**
 * @brief  Get the status of the com link
 * @retval 1 is connected, 0 if not.
 */
int com_is_connected;

/**
 * @brief  Get connection hardware type
 * @retval com_link_type
 */
com_link_type com_get_link_type();

/**
 * @brief  Interrupt incomming connection handeler
 * @retval None
 */
void _com_int_incomming_connection();

/**
 * @brief  Tramsmission handeler. Do not call directly, blocks CPU.
 * @retval None
 */
void _com_transmissionQueueHandeler();

/**
 * @brief  directly transmit a package. Do not call directly. Use `com_send()` instead.
 * @param  package: package to be send
 * @retval None
 */
void _com_transmit(_com_transmitpackage_t package);

/**
 * @brief  Strips `com_datapackage_t` to `_com_transmitpackage_t`
 * @param  datap: paclage to be stripped.
 * @retval _com_transmitpackage_t
 */
_com_transmitpackage_t _com_datapackage2transmitpackage(com_datapackage_t datap);


#endif // ARUNA_COM_H