//
// Created by noeel on 7-1-19.
//

#include <drivers/Com.h>


Com::Com() {

}

com_err Com::start() {
    return this->start(pickDriver());
}

com_err Com::start(ComDriver *driver) {
    switch (get_status()) {
        case COM_RUNNING:
            return COM_ERR_NOT_STOPPED;
            break;
        case COM_PAUSED:
//            TODO is dit slim? kan ongwenst gedrag vertonen als je van driver wilt wisselen.
            return resume();
        default:
            break;
    }
    setDriver(*driver);
    getDriver()->start();
    this->set_status(COM_RUNNING);
//    TODO in freeRTOS thread opstarten.
    this->transmissionQueueHandeler();
//    TODO error handeling
    return COM_OK;
}

com_err Com::stop() {
    switch (get_status()) {
        case COM_STOPPED:
            return COM_ERR_NOT_STARTED;
        default:
            break;
    }
    getDriver()->stop();
    this->set_status(COM_STOPPED);
//    TODO destroy threads and clear all arrays and queues
//      TODO error handeling
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
    this->set_status(COM_PAUSED);
//    TODO pause queuehandler thread.
//      TODO error handeling
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
    this->set_status(COM_RUNNING);
//    TODO resume threads
//  TODO error handeling
    return COM_OK;
}

com_err Com::register_channel(com_endpoint_t &endpoint) {
    if( channels->find(endpoint) != channels->end())
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
//    TODO checken of com_datap endpoint wel bestaat.
    switch (data.com_endpoint->priority) {
//        TODO error handeling
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

void Com::getName(char buffer[COM_ENDPOINT_NAME_SIZE]) {
    this->getDriver()->getName(buffer);
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

ComDriver *Com::pickDriver() {
//    bestpick initalisren omdat hij anders een lege terug kan geven.
    ComDriver *bestPick = driverCandidates[0];
    unsigned int bestPickScore = 0;
    unsigned int s = 0;
    for (int i = 0; i < (sizeof(driverCandidates) / sizeof(*driverCandidates)); ++i) {
        s = rateDriver(*driverCandidates[i]);
        if (s > bestPickScore) {
            bestPick = driverCandidates[i];
            bestPickScore = s;
        }
    }
//    TODO error handeling als er geen driver candidates zijn.
    return bestPick;
}

unsigned int Com::rateDriver(ComDriver &driver) {
    unsigned int score = 0;
    driver.start();
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

    driver.stop();
    return score;
}

void Com::transmissionQueueHandeler() {
//    TODO deze vergelijking kost misschien te veel tijd.
    while (this->get_status() == COM_RUNNING) {
        if (!transmission1_queue->empty()) {
            this->getDriver()->transmit(transmission1_queue->front(), 1);
            transmission1_queue->pop();
        }
        while (!transmission0_queue->empty()) {
            this->getDriver()->transmit(transmission0_queue->front(), 0);
            transmission0_queue->pop();
        }
        if (!transmission1_queue->empty()) {
            this->getDriver()->transmit(transmission1_queue->front(), 1);
            transmission1_queue->pop();
        }
        if (!transmission2_queue->empty()) {
            this->getDriver()->transmit(transmission2_queue->front(), 2);
            transmission2_queue->pop();
        }
    }
//    deze methode doet nu aan busy waiting, wat natuurlijk niet idiaal is.
//    deze recursie kost misschien heel veel geheugen.
    return this->transmissionQueueHandeler();
}