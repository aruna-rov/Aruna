//
// Created by noeel on 7-1-19.
//

#include <drivers/Com.h>
#include <esp_log.h>


Com::Com() {
}

com_err Com::start() {
    auto dr = pickDriver();
    if (std::get<1>(dr) == COM_OK) {
        return this->start(std::get<0>(dr));
    }
    return std::get<1>(dr);
}

com_err Com::start(ComDriver *driver) {
    switch (get_status()) {
        case COM_RUNNING:
//            TODO COM_ERR_RUNNING betere error?
            return COM_ERR_NOT_STOPPED;
        case COM_PAUSED:
//            TODO is dit slim? kan ongwenst gedrag vertonen als je van driver wilt wisselen.
            return resume();
        default:
            break;
    }
    com_err driver_err;
    setDriver(*driver);
    driver_err = getDriver()->start();
    if(driver_err != COM_OK)
        return driver_err;

//    TODO priority goed zetten.
//      Gaat zoiezo mis als je meerdere Com objecten probeerd aan temaken.
//      Misschien kunnen we beter de hele class static maken?
    if(xTaskCreate(transmissionQueueHandeler,
            "COM_transmissionQueueHandeler",
            configMINIMAL_STACK_SIZE,
            this,
            tskIDLE_PRIORITY,
            &this->transmissionQueueHandeler_task ) != pdPASS) {
        driver_err = getDriver()->stop();
        if (driver_err != COM_OK)
            ESP_LOGW(LOG_TAG, "Driver failed to stop: %d", driver_err);
//        TODO driver unsetten en destroyen.
        return COM_ERR_TASK_FAILED;
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
    com_err driver_err;
    driver_err = getDriver()->stop();
    vTaskDelete(transmissionQueueHandeler_task);
    this->set_status(COM_STOPPED);
    if (driver_err != COM_OK)
        return driver_err;
//    TODO destroy threads and clear all arrays and queues
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
//            TODO is start aanroepen slim? Als je eerst start(driver) hebt gedaan kan het ongewenste dingen doen.
//              TODO kan nu ook een null pointer geven als je hem vanuit een koude start `resume` doet.
            return start(getDriver());
        case COM_RUNNING:
            return COM_ERR_NOT_PAUSED;
        default:
            break;
    }
    vTaskResume(transmissionQueueHandeler_task);
    this->set_status(COM_RUNNING);
    return COM_OK;
}

com_err Com::register_channel(com_endpoint_t &endpoint) {
//    TODO testen of deze check wel werkt.
    if (channels->find(endpoint) != channels->end())
        return COM_ERR_CHANNEL_EXISTS;
    if (channels->insert(endpoint).second)
        return COM_OK;
    else
        return COM_ERR_BUFFER_OVERFLOW;
//    TODO handeler fixen.
}

com_err Com::unregister_channel(com_endpoint_t &endpoint) {
    if (channels->erase(endpoint))
        return COM_OK;
    else
        return COM_ERR_NO_CHANNEL;
}

com_err Com::send(com_datapackage_t data) {
    if (channels->find(*data.com_endpoint) == channels->end()) {
        return COM_ERR_NO_CHANNEL;
    }
    switch (data.com_endpoint->priority) {
//        TODO error detectie op dat de queue vol zit.
//        Maar queue->push() return void :( ?
        case 0:
            transmission0_queue->push(this->datapackage2transmitpackage(data));
            break;
        case 1:
            transmission1_queue->push(this->datapackage2transmitpackage(data));
            break;
        case 2:
            transmission2_queue->push(this->datapackage2transmitpackage(data));
            break;
        default:
//            TODO COM__ERR_INVALID.. documentatie toevoegen.
            return COM_ERR_INVALID_PARAMETERS;
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

char* Com::getName() {
    return this->getDriver()->getName();
}

com_err Com::get_channels(char buffer[COM_ENDPOINT_NAME_SIZE][CHANNEL_BUFFER_SIZE]) {
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
    if (driver.start() != COM_OK)
        return score;
    if (driver.isHardwareConnected())
        score += 10;
    if (!driver.isEndpointConnected())
        return score;

    switch (driver.getLinkType()) {
        case COM_WIRED:
            score *= 3;
            break;
        case COM_RADIO:
            score *= 2;
            break;
        case COM_NONE:
//            TODO is COM_NONE eingelijk wel mogelijk?
            return score;
    }
    score *= (int) driver.getSpeed() / 100;

    if (driver.isRealTime())
        score *= 6;

    if(driver.stop() != COM_OK)
        return score / 1000;
    return score;
}

void Com::transmissionQueueHandeler() {
    int schedularCount = 0;
    while (1) {
        if (!transmission0_queue->empty()) {
            if (this->getDriver()->transmit(transmission0_queue->front(), 0) == COM_OK)
                transmission0_queue->pop();
        } else {
            if (schedularCount < 2 && !transmission1_queue->empty()) {
                if(this->getDriver()->transmit(transmission1_queue->front(), 1) == COM_OK) {
                    transmission1_queue->pop();
                    schedularCount++;
                }
            }
            else if (!transmission2_queue->empty()) {
                if(this->getDriver()->transmit(transmission2_queue->front(), 2) == COM_OK) {
                    transmission2_queue->pop();
                    schedularCount = 0;
                }
            }
        }
    }
}

void Com::transmissionQueueHandeler(void *_this) {
    static_cast<Com*>(_this)->transmissionQueueHandeler();
}


com_err Com::incoming_connection(com_transmitpackage_t package) {

/*
 *    TODO O(N) is eigenlijk te langzaam.
 *    Als we het meteen kunnen opzoeken door ook nog een set te maken (of hashing table) van de namen
 *    scheelt dat ons heel veel tijd want dan is het O(1)!
 */
    for (const auto &channel : *channels) {
        if (channel.name == package.endpoint_name) {
            channel.handeler(package.data);
            return COM_OK;
        }
    }
    return COM_ERR_NO_CHANNEL;

}

com_err Com::register_candidate_driver(ComDriver *driver) {
//    comdriver moet eigenlijk een referentie zijn en niet een levend object.
    char name[CHANNEL_BUFFER_SIZE];

//    TODO print maar 1 letter.
    ESP_LOGD(LOG_TAG, "registering driver: %s", driver->getName());
    if (driverCandidates.find(driver) != driverCandidates.end()) {
        return COM_ERR_DRIVER_EXISTS;
    }
    if (!driverCandidates.insert(driver).second)
        return COM_ERR_BUFFER_OVERFLOW;
    if (get_status() == COM_RUNNING) {
        selectDriverTask();
    }
    return COM_OK;
}


com_err Com::unregister_candidate_driver(ComDriver *driver) {
    if (driverCandidates.erase(driver)) {
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
    } else
//        TODO status aanpassen ofzo.
        ESP_LOGE(LOG_TAG, "Failed to pick new driver: %d",std::get<1>(dr) );
    vTaskDelete(NULL);
}

void Com::_selectDriverTask(void * _this) {
    static_cast<Com*>(_this)->_selectDriverTask();

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

