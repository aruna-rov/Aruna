//
// Created by noeel on 7-1-19.
//

#include "app/Com.h"
#include <esp_log.h>

Com::Com() {
}

com_err Com::start() {
//    pick a driver
    auto dr = pickDriver();
    if (std::get<1>(dr) == COM_OK) {
//        if driver is found start COM with driver as paramter
        return this->start(std::get<0>(dr));
    }
//    return pickDriver error.
    return std::get<1>(dr);
}

com_err Com::start(ComDriver *driver) {
    switch (get_status()) {
        case COM_RUNNING:
            return COM_ERR_NOT_STOPPED;
        case COM_PAUSED:
//            TODO is dit slim? kan ongwenst gedrag vertonen als je van driver wilt wisselen.
            return resume();
        default:
            break;
    }
//    set the driver
    com_err driver_err;
    setDriver(*driver);
    driver_err = getDriver()->start();
    if (driver_err != COM_OK)
        return driver_err;

//    TODO priority goed zetten.
//      Gaat zoiezo mis als je meerdere Com objecten probeerd aan temaken.
//      Misschien kunnen we beter de hele class static maken?
    if (xTaskCreate(transmissionQueueHandeler,
                    "COM_transmissionQueueHandeler",
                    (2048),
                    this,
                    tskIDLE_PRIORITY,
                    &this->transmissionQueueHandeler_task) != pdPASS) {
//        stop when task failes
        driver_err = getDriver()->stop();
        if (driver_err != COM_OK)
            ESP_LOGE(LOG_TAG, "Driver failed to stop: %d", driver_err);
//        TODO driver unsetten en destroyen.
        return COM_ERR_TASK_FAILED;
    }
//    set status to running if all succeeded

    transmission_queue_set = xQueueCreateSet(TRANSMIT_QUEUE_BUFFER_SIZE + sizeof(com_transmitpackage_t) *
                                                                          (sizeof(transmission_queue) /
                                                                           sizeof(*transmission_queue)));
    for (int i = 0; i < (sizeof(transmission_queue) / sizeof(*transmission_queue)); ++i) {
        transmission_queue[i] = xQueueCreate(TRANSMIT_QUEUE_BUFFER_SIZE, sizeof(com_transmitpackage_t));
        if(xQueueAddToSet(transmission_queue[i], transmission_queue_set) != pdPASS)
            ESP_LOGW(LOG_TAG, "failed to add queue[%d] to set", i);
    }
    this->set_status(COM_RUNNING);

    return COM_OK;
}

com_err Com::stop() {
    switch (get_status()) {
        case COM_STOPPED:
            return COM_ERR_NOT_STARTED;
        default:
            break;
    }
//    stop all
    com_err driver_err;
    driver_err = getDriver()->stop();
    vTaskDelete(transmissionQueueHandeler_task);
    this->set_status(COM_STOPPED);

    for (int i = 0; i < (sizeof(transmission_queue) / sizeof(*transmission_queue)); ++i) {
        xQueueReset(transmission_queue[i]);
        if (xQueueRemoveFromSet(transmission_queue[i], transmission_queue_set) != pdPASS)
            ESP_LOGW(LOG_TAG, "failed to remove queue[%d] from set", i);
    }
    if (driver_err != COM_OK)
        return driver_err;
    return COM_OK;
}

com_err Com::pause() {
    switch (get_status()) {
        case COM_STOPPED:
        case COM_PAUSED:
            return COM_ERR_NOT_STARTED;
        default:
            break;
    }
    vTaskSuspend(transmissionQueueHandeler_task);
    this->set_status(COM_PAUSED);
    return COM_OK;
}

com_err Com::resume() {
    switch (get_status()) {
        case COM_STOPPED:
            return COM_ERR_NOT_STARTED;
        case COM_RUNNING:
            return COM_ERR_NOT_PAUSED;
        default:
            break;
    }
    vTaskResume(transmissionQueueHandeler_task);
    this->set_status(COM_RUNNING);
    return COM_OK;
}

com_err Com::register_channel(com_channel_t *channel) {
/*
 * TODO paramters should consist of the paramters of com_channel together with a handeler.
 * handeler can then later be used for sending and accessing Rx array
 */

//    TODO testen of deze check wel werkt.
    if (channels.find(*channel) != channels.end())
        return COM_ERR_CHANNEL_EXISTS;
    if (channels.insert(*channel).second) {
        *channel->handeler = xQueueCreate(TRANSMIT_QUEUE_BUFFER_SIZE, sizeof(com_transmitpackage_t));
        return COM_OK;
    } else
        return COM_ERR_BUFFER_OVERFLOW;
//    TODO handeler fixen.
}

com_err Com::unregister_channel(com_channel_t &channel) {
    if (channels.erase(channel))
        return COM_OK;
    else
        return COM_ERR_NO_CHANNEL;
}

com_err Com::send(com_channel_t *channel, com_port_t to_port, com_data_t data, size_t data_size) {
    if (get_status() != COM_RUNNING) return COM_ERR_NOT_STARTED;
    if (channels.find(*channel) == channels.end()) {
        return COM_ERR_NO_CHANNEL;
    }
    //        datasize length
    size_t ds_l;
    //        datasize pointer
    size_t ds_p = 0;

    com_transmitpackage_t tp;
    tp.to_port = to_port;
    tp.from_port = channel->port;

    if (channel->priority >= sizeof(transmission_queue) / sizeof(*transmission_queue) ||
        channel->priority < 0)
        return COM_ERR_INVALID_PARAMETERS;

//    loop until all data in send
    while (data_size > 0) {
        ds_l = (data_size >= COM_DATA_SIZE) ? COM_DATA_SIZE : data_size;

        memcpy(tp.data, &data[ds_p], ds_l);
        tp.data_lenght = ds_l;

        ds_p += ds_l;
        data_size -= ds_l;

        if (xQueueSend(transmission_queue[channel->priority], &tp, 0) != pdPASS)
            return COM_ERR_BUFFER_OVERFLOW;

//        clear any left over data
        bzero(tp.data, tp.data_lenght);
    }
    return COM_OK;
}

bool Com::is_connected() {
    return this->getDriver()->isEndpointConnected();
}

com_status Com::get_status() {
    return this->status;
}

void Com::set_status(com_status status) {
    this->status = status;
}

com_link_t Com::get_link_type() {
    return this->getDriver()->getLinkType();
}

void Com::getName(char *buffer) {
    this->getDriver()->getName(buffer);
}

char *Com::getName() {
    return this->getDriver()->getName();
}

com_err Com::get_channels(char *buffer) {
// TODO
    return COM_OK;
}

unsigned int Com::get_speed() {
    return this->getDriver()->getSpeed();
}

ComDriver *Com::getDriver() {
    return this->driver;
}

void Com::setDriver(ComDriver &driver) {
    this->driver = &driver;
}

std::tuple<ComDriver *, com_err> Com::pickDriver() {
//    bestpick initalisren omdat hij anders een lege terug kan geven.
    ComDriver *bestPick = nullptr;
    unsigned int bestPickScore = 0;
    unsigned int s = 0;
    if (driverCandidates.empty())
        return std::make_tuple(bestPick, COM_ERR_NO_DRIVER);
    for (auto driverCandidate : driverCandidates) {
        s = rateDriver(*driverCandidate);
        if (s > bestPickScore) {
            bestPick = driverCandidate;
            bestPickScore = s;
        }
//        TODO drivers die het niet zijn geworden moeten worden gedeleted.
    }
    return std::make_tuple(bestPick, COM_OK);
}

unsigned int Com::rateDriver(ComDriver &driver) {
    unsigned int score = 0;
    com_err drivstrt = driver.start();
    if (drivstrt != COM_OK) {
        ESP_LOGW(LOG_TAG, "driver:%s, failed to start: %d", driver.getName(), drivstrt);
        return score;
    }
    if (!driver.isHardwareConnected())
        return score;
    else
        score += 10;
    if (!driver.isEndpointConnected())
        return score;
    else
        score += 15;

    switch (driver.getLinkType()) {
        case COM_WIRED:
            score *= 3;
            break;
        case COM_RADIO:
            score *= 2;
            break;
        case COM_NONE:
//            TODO is COM_NONE even posible?
            return score;
    }
    score *= (int) driver.getSpeed() / 100;

    if (driver.isRealTime())
        score *= 6;

    com_err drivstp = driver.stop();
    if (drivstp != COM_OK) {
        ESP_LOGW(LOG_TAG, "driver: %s, failed to stop: %d", driver.getName(), drivstp);
        return score / 1000;
    }
    return score;
}

void Com::transmissionQueueHandeler() {
    int schedularCount = 0;
    QueueSetMemberHandle_t xActivatedMember;
    com_transmitpackage_t transpack;
    com_err transmit_msg = COM_FAIL;
    while (1) {
        xActivatedMember = xQueueSelectFromSet(transmission_queue_set, (portTickType) portMAX_DELAY);
        if (xActivatedMember == transmission_queue[0]) {
            xQueueReceive(transmission_queue[0], &transpack, 0);
            transmit_msg = this->getDriver()->transmit(transpack, 0);
        } else {
            if (schedularCount < 2 && xActivatedMember == transmission_queue[1]) {
                xQueueReceive(transmission_queue[1], &transpack, 0);
                transmit_msg = this->getDriver()->transmit(transpack, 1);

                schedularCount++;
            } else if (xActivatedMember == transmission_queue[2]) {
                xQueueReceive(transmission_queue[2], &transpack, 0);
                transmit_msg = this->getDriver()->transmit(transpack, 2);

                schedularCount = 0;
            }
        }
        if (transmit_msg != COM_OK) {
            ESP_LOGW(LOG_TAG, "transmit of:%d, to:%d, failed: %d", transpack.from_port, transpack.to_port,
                     transmit_msg);
            ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, transpack.data, sizeof(transpack.data), ESP_LOG_WARN);
        }
    }
}

void Com::transmissionQueueHandeler(void *_this) {
//    static overload to enable RTOS task of an object
    static_cast<Com *>(_this)->transmissionQueueHandeler();
}


com_err Com::incoming_connection(com_transmitpackage_t package) {
// TODO use com_bin_t as parameter instead if com_transmitpackage
/*
 *    TODO O(N) is eigenlijk te langzaam.
 *    Als we het meteen kunnen opzoeken door ook nog een set te maken (of hashing table) van de namen
 *    scheelt dat ons heel veel tijd want dan is het O(1)!
 */
    if (get_status() != COM_RUNNING) return COM_ERR_NOT_STARTED;
    ESP_LOGV(LOG_TAG, "incoming connection");

    for (const auto &channel : channels) {
        if (channel.port == package.to_port) {
            xQueueSend(*channel.handeler, &package, 1000);
            return COM_OK;
        }
    }
    return COM_ERR_NO_CHANNEL;

}

com_err Com::register_candidate_driver(ComDriver *driver) {
//    comdriver moet eigenlijk een referentie zijn en niet een levend object.

    ESP_LOGD(LOG_TAG, "registering driver: %s", driver->getName());
    if (driverCandidates.find(driver) != driverCandidates.end()) {
        return COM_ERR_DRIVER_EXISTS;
    }
    if (!driverCandidates.insert(driver).second)
        return COM_ERR_BUFFER_OVERFLOW;
//    select new driver if com is running
    if (get_status() == COM_RUNNING) {
//        TODO its better to rate this new driver and compare its score to the current driver.
        selectDriverTask();
    }
    return COM_OK;
}


com_err Com::unregister_candidate_driver(ComDriver *driver) {
    if (driverCandidates.erase(driver)) {
//        if we delete our own driver, search for a new one
        if (this->driver == driver)
            selectDriverTask();
        return COM_OK;
    } else
        return COM_ERR_NO_DRIVER;
}

void Com::_selectDriverTask() {
    auto dr = pickDriver();
    if (std::get<1>(dr) == COM_OK) {
        setDriver(*std::get<0>(dr));
    } else {
        ESP_LOGE(LOG_TAG, "Failed to pick new driver: %d", std::get<1>(dr));
        COM.stop();
    }

    vTaskDelete(NULL);
}

void Com::_selectDriverTask(void *_this) {
//    static overload, for RTOS to work,
    static_cast<Com *>(_this)->_selectDriverTask();

}

void Com::selectDriverTask() {
//    TODO priority hoger zetten en misschien nog een timeout ofzo toevoegen.
    xTaskCreate(_selectDriverTask,
                "COM_selectDriverTask",
                configMINIMAL_STACK_SIZE,
                this,
                tskIDLE_PRIORITY,
                NULL);
}
