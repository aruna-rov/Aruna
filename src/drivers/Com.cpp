//
// Created by noeel on 7-1-19.
//

#include <drivers/Com.h>


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
    setDriver(*driver);
//    TODO error handeling
    getDriver()->start();
    this->set_status(COM_RUNNING);
//    TODO in freeRTOS thread opstarten.
//    this->transmissionQueueHandeler();
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
//    TODO com_Err error handeling
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
    }
    return std::make_tuple(bestPick, COM_OK);
}

unsigned int Com::rateDriver(ComDriver &driver) {
//    TODO error handeling
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
    int schedularCount = 0;
    while (this->get_status() == COM_RUNNING) {
//        TODO error handeling bij de transmit().

        if (!transmission0_queue->empty()) {
            this->getDriver()->transmit(transmission0_queue->front(), 0);
            transmission0_queue->pop();
        } else {
            if (schedularCount < 2 && !transmission1_queue->empty()) {
                this->getDriver()->transmit(transmission1_queue->front(), 1);
                transmission1_queue->pop();
                schedularCount++;
            }
            else if (!transmission2_queue->empty()) {
                this->getDriver()->transmit(transmission2_queue->front(), 2);
                transmission2_queue->pop();
                schedularCount = 0;
            }
        }
    }
//    deze methode doet nu aan busy waiting, wat natuurlijk niet idiaal is.
//    deze recursie kost misschien heel veel geheugen.
    return this->transmissionQueueHandeler();
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
    if (driverCandidates.find(driver) != driverCandidates.end()) {
        return COM_ERR_DRIVER_EXISTS;
    }
    if (!driverCandidates.insert(driver).second)
        return COM_ERR_BUFFER_OVERFLOW;
//    if (get_status() == COM_RUNNING) {
////    TODO    pickDriver moet in een aparte thread.
////          error correctie
//        pickDriver();
//    }
    return COM_OK;
}

com_err Com::unregister_candidate_driver(ComDriver *driver) {
    if (driverCandidates.erase(driver)) {
//        if (this->driver == driver)
////        TODO pickdriver moet in aparte thread.
////          error correctie
//            pickDriver();
        return COM_OK;
    } else
        return COM_ERR_NO_DRIVER;
}
