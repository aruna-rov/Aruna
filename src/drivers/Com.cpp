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
//    TODO testen, denk niet dat dit werkt namelijk. Want de size is altijd 32. Misschien linked list of vector gebruiken?
    size_t size = (sizeof(this->channels) / sizeof(*this->channels));
    if (size > this->CHANNEL_BUFFER_SIZE)
        return COM_ERR_BUFFER_OVERFLOW;
//    dit creerd volgens mij memory leaks.
//    this->channels[size++] = &endpoint;
    return COM_OK;
}

com_err Com::unregister_channel(com_endpoint_t &endpoint) {
//    channel array plekje word nu verwijderd, maar kan niet opnieuw worden gevuld.
    for (int i = 0; i < (sizeof(this->channels) / sizeof(*this->channels)); ++i) {
        if (this->channels[i]->name == endpoint.name) {
            com_endpoint_t empty = {
                    "0",
                    0,
//                    geeft compiler warning omdat hij geen handeler heeft.
            };
//            dit maakt volgens mij memory leaks.
            this->channels[i] = &empty;
        }
    }
//    TODO error handeling
    return COM_OK;
}

com_err Com::send(com_datapackage_t data) {
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
    std::queue<com_transmitpackage_t> q0;
//    TODO transmissionN_queue moet een queue object zijn, anders is dit erg lastig te programeren.
}