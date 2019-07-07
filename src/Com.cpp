//
// Created by noeel on 7-1-19.
//

#include "aruna/Com.h"
#include <esp_log.h>
#include <math.h>
#include <aruna/drivers/com/ComDriver.h>
namespace aruna {
using namespace drivers::com;
    Com::Com() {
    }


    Com::err_t Com::start() {
//    pick a driver
        auto dr = pickDriver();
        if (std::get<1>(dr) == Com::err_t::OK) {
//        if driver is found start COM with driver as paramter
            return this->start(std::get<0>(dr));
        }
//    return pickDriver error.
        return std::get<1>(dr);
    }

    Com::err_t Com::start(drivers::com::ComDriver *driver) {
        switch (get_status()) {
            case status_t::RUNNING:
                return err_t::NOT_STOPPED;
            case status_t::PAUSED:
//            TODO is dit slim? kan ongwenst gedrag vertonen als je van driver wilt wisselen.
                return resume();
            default:
                break;
        }
//    set the driver
        err_t driver_err;
        setDriver(*driver);
        driver_err = getDriver()->start();
        if (driver_err != err_t::OK)
            return driver_err;
        bzero(&times_tried, N_COUNT_MAX + 1);
        transmission_queue_set = xQueueCreateSet(TRANSMIT_QUEUE_BUFFER_SIZE + sizeof(transmitpackage_t) *
                                                                              (sizeof(transmission_queue) /
                                                                               sizeof(*transmission_queue)));
        for (int i = 0; i < (sizeof(transmission_queue) / sizeof(*transmission_queue)); ++i) {
            transmission_queue[i] = xQueueCreate(TRANSMIT_QUEUE_BUFFER_SIZE, sizeof(transmitpackage_t));
            if (xQueueAddToSet(transmission_queue[i], transmission_queue_set) != pdPASS)
                ESP_LOGW(LOG_TAG, "failed to add queue[%d] to set", i);
        }
//    TODO priority goed zetten.
//      Gaat zoiezo mis als je meerdere Com objecten probeerd aan temaken.
//      Misschien kunnen we beter de hele class static maken?
        if (xTaskCreate(transmissionQueueHandeler,
                        "COM_transmissionQueueHandeler",
                        (4096),
                        this,
                        20,
                        &this->transmissionQueueHandeler_task
        ) != pdPASS) {
//        stop when task failes
            driver_err = getDriver()->stop();
            if (driver_err != err_t::OK)
                ESP_LOGE(LOG_TAG, "Driver failed to stop: %d", (int) driver_err);
//        TODO driver unsetten en destroyen.
            return err_t::TASK_FAILED;
        }
//    set status to running if all succeeded

        this->set_status(status_t::RUNNING);

        return err_t::OK;
    }

    Com::err_t Com::stop() {
        switch (get_status()) {
            case status_t::STOPPED:
                return err_t::NOT_STARTED;
            default:
                break;
        }
//    stop all
        err_t driver_err;
        driver_err = getDriver()->stop();
        vTaskDelete(transmissionQueueHandeler_task);
        this->set_status(status_t::STOPPED);

        for (int i = 0; i < (sizeof(transmission_queue) / sizeof(*transmission_queue)); ++i) {
            xQueueReset(transmission_queue[i]);
            if (xQueueRemoveFromSet(transmission_queue[i], transmission_queue_set) != pdPASS)
                ESP_LOGW(LOG_TAG, "failed to remove queue[%d] from set", i);
        }
        if (driver_err != err_t::OK)
            return driver_err;
        return err_t::OK;
    }

    Com::err_t Com::pause() {
        switch (get_status()) {
            case status_t::STOPPED:
            case status_t::PAUSED:
                return err_t::NOT_STARTED;
            default:
                break;
        }
        vTaskSuspend(transmissionQueueHandeler_task);
        this->set_status(status_t::PAUSED);
        return err_t::OK;
    }

    Com::err_t Com::resume() {
        switch (get_status()) {
            case status_t::STOPPED:
                return err_t::NOT_STARTED;
            case status_t::RUNNING:
                return err_t::NOT_PAUSED;
            default:
                break;
        }
        vTaskResume(transmissionQueueHandeler_task);
        this->set_status(status_t::RUNNING);
        return err_t::OK;
    }

    Com::err_t Com::register_channel(channel_t *channel) {
/*
 * TODO paramters should consist of the paramters of com_channel together with a handeler.
 * handeler can then later be used for sending and accessing Rx array
 */

//    TODO testen of deze check wel werkt.
        if (channels.find(*channel) != channels.end())
            return err_t::CHANNEL_EXISTS;
        if (channels.insert(*channel).second) {
            *channel->handeler = xQueueCreate(TRANSMIT_QUEUE_BUFFER_SIZE, sizeof(transmitpackage_t));
            return err_t::OK;
        } else
            return err_t::BUFFER_OVERFLOW;
//    TODO handeler fixen.
    }

    Com::err_t Com::unregister_channel(channel_t &channel) {
        if (channels.erase(channel))
            return err_t::OK;
        else
            return err_t::NO_CHANNEL;
    }

    Com::err_t
    Com::send(channel_t *channel, port_t to_port, uint8_t *data, size_t data_size, bool wait_for_ack) {
        if (get_status() != status_t::RUNNING) return err_t::NOT_STARTED;
        if (channels.find(*channel) == channels.end()) {
            return err_t::NO_CHANNEL;
        }
        uint packages = ceil((double) data_size / (double) MAX_DATA_SIZE);
//	how many packages are we waiting for?
        uint8_t n_to_wait_for = 0;
        //        datasize length
        size_t ds_l;
        //        datasize pointer
        size_t ds_p = 0;


        if (channel->priority >= sizeof(transmission_queue) / sizeof(*transmission_queue) ||
            channel->priority < 0)
            return err_t::INVALID_PARAMETERS;

        if (!getDriver()->isEndpointConnected())
            return err_t::NO_CONNECTION;

        auto tp = (transmitpackage_t *) calloc(packages, sizeof(transmitpackage_t));
//	TODO check if tp is allocated
//    loop until all data in send
        while (data_size > 0) {
//		static data
            tp[n_to_wait_for].to_port = to_port;
            tp[n_to_wait_for].from_port = channel->port;
            tp[n_to_wait_for].priority = channel->priority;
            tp[n_to_wait_for].n = 0;
            tp[n_to_wait_for].sending_task = xTaskGetCurrentTaskHandle();
            tp[n_to_wait_for].notify_on_ack = wait_for_ack;

            tp[n_to_wait_for].data_transmitting = &data[ds_p];

            ds_l = (data_size >= MAX_DATA_SIZE) ? MAX_DATA_SIZE : data_size;

            tp[n_to_wait_for].data_lenght = ds_l;
            tp[n_to_wait_for].size = ds_l + transmitpackage_t::HEADER_SIZE;

            ds_p += ds_l;
            data_size -= ds_l;
            ESP_LOGV(LOG_TAG, "sending from: %d to: %d, data: %s", tp[n_to_wait_for].from_port,
                     tp[n_to_wait_for].to_port, tp[n_to_wait_for].data_transmitting);
            n_to_wait_for++;
            if (xQueueSend(transmission_queue[channel->priority], tp, 0) != pdPASS) {
                free(tp);
                return err_t::BUFFER_OVERFLOW;
            }

//        clear any left over data
        }
        if (wait_for_ack) {
            while (n_to_wait_for > 0) {
                if (ulTaskNotifyTake(pdTRUE, ACK_TIMEOUT * MAX_TRIES))
                    n_to_wait_for--;
                else {
                    free(tp);
                    return err_t::NO_RESPONSE;
                }
            }

        }
        free(tp);
        return err_t::OK;
    }

    bool Com::is_connected() {
        return this->getDriver()->isEndpointConnected();
    }

    Com::status_t Com::get_status() {
        return this->status;
    }

    void Com::set_status(status_t status) {
        this->status = status;
    }

    Com::link_t Com::get_link_type() {
        return this->getDriver()->getLinkType();
    }

    void Com::getName(char *buffer) {
        this->getDriver()->getName(buffer);
    }

    char *Com::getName() {
        return this->getDriver()->getName();
    }

    Com::err_t Com::get_channels(char *buffer) {
// TODO
        return err_t::OK;
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

    std::tuple<ComDriver *, Com::err_t> Com::pickDriver() {
//    bestpick initalisren omdat hij anders een lege terug kan geven.
        ComDriver *bestPick = nullptr;
        unsigned int bestPickScore = 0;
        unsigned int s = 0;
        if (driverCandidates.empty())
            return std::make_tuple(bestPick, err_t::NO_DRIVER);
        for (auto driverCandidate : driverCandidates) {
            s = rateDriver(*driverCandidate);
            if (s > bestPickScore) {
                bestPick = driverCandidate;
                bestPickScore = s;
            }
//        TODO drivers die het niet zijn geworden moeten worden gedeleted.
        }
        return std::make_tuple(bestPick, err_t::OK);
    }

    unsigned int Com::rateDriver(ComDriver &driver) {
        unsigned int score = 0;
        err_t drivstrt = driver.start();
        if (drivstrt != err_t::OK) {
            ESP_LOGW(LOG_TAG, "driver:%s, failed to start: %d", driver.getName(), (int) drivstrt);
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
            case link_t::WIRED:
                score *= 3;
                break;
            case link_t::RADIO:
                score *= 2;
                break;
            case link_t::NONE:
//            TODO is COM_NONE even posible?
                return score;
        }
        score *= (int) driver.getSpeed() / 100;

        if (driver.isRealTime())
            score *= 6;

        err_t drivstp = driver.stop();
        if (drivstp != err_t::OK) {
            ESP_LOGW(LOG_TAG, "driver: %s, failed to stop: %d", driver.getName(), (int) drivstp);
            return score / 1000;
        }
        return score;
    }

    void Com::transmissionQueueHandeler() {
//	int schedularCount = 0;
        QueueSetMemberHandle_t xActivatedMember;
        transmitpackage_t transpack;
        err_t transmit_msg = err_t::FAIL;

        while (1) {
            xActivatedMember = xQueueSelectFromSet(transmission_queue_set, (portTickType) portMAX_DELAY);
            if (xActivatedMember == transmission_queue[0]) {
                xQueueReceive(transmission_queue[0], &transpack, 0);
                transmit_msg = transmit(transpack);
            } else {
                if (
//						TODO fix schedular
//						schedularCount < 2 &&
                        xActivatedMember == transmission_queue[1]) {
                    xQueueReceive(transmission_queue[1], &transpack, 0);
                    transmit_msg = transmit(transpack);

//					schedularCount++;
                } else if (xActivatedMember == transmission_queue[2]) {
                    xQueueReceive(transmission_queue[2], &transpack, 0);
                    transmit_msg = transmit(transpack);

//					schedularCount = 0;
                }
            }
            if (transmit_msg != err_t::OK) {
                ESP_LOGW(LOG_TAG, "transmit of: %d, to: %d, failed: 0x%X", transpack.from_port, transpack.to_port,
                         (uint8_t) transmit_msg);
                ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, &transpack.data_transmitting, transpack.data_lenght, ESP_LOG_WARN);
            }
        }
        vTaskDelete(NULL);
    }

    void Com::acknowledge_handler_task(transmitpackage_t transmitpackage_to_watch) {
        bool success = false;
        ESP_LOGV(LOG_TAG, "new task watcher %d", transmitpackage_to_watch.n);
        times_tried[transmitpackage_to_watch.n] = 1;
        while (1) {

            if (ulTaskNotifyTake(pdTRUE, (portTickType) ACK_TIMEOUT)) {
                success = true;
                break;
            }


            if (times_tried[transmitpackage_to_watch.n] >= MAX_TRIES) {
                ESP_LOGE(LOG_TAG, "ack[%d] permanently failed. From: %d to: %d", transmitpackage_to_watch.n,
                         transmitpackage_to_watch.from_port, transmitpackage_to_watch.to_port);
                ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, transmitpackage_to_watch.data_transmitting,
                                       transmitpackage_to_watch.data_lenght,
                                       ESP_LOG_VERBOSE);
                break;
            } else {
                ESP_LOGV("COM", "ack[%d] timeout %d. From: %d to: %d", transmitpackage_to_watch.n,
                         times_tried[transmitpackage_to_watch.n], transmitpackage_to_watch.from_port,
                         transmitpackage_to_watch.to_port);
                transmitpackage_to_watch.resend = true;
                xQueueSend(transmission_queue[transmitpackage_to_watch.priority], &transmitpackage_to_watch, 0);
                times_tried[transmitpackage_to_watch.n]++;
            }
        }

        times_tried[transmitpackage_to_watch.n] = 0;
        if (success && transmitpackage_to_watch.notify_on_ack)
            xTaskNotifyGive(transmitpackage_to_watch.sending_task);

//	notify queue handeler that we are done.
        xTaskNotify(transmissionQueueHandeler_task, transmitpackage_to_watch.n, eSetValueWithOverwrite);
        vTaskDelete(NULL);
    }


    void Com::_acknowledge_handler_task(void *handle) {
//    static overload to enable RTOS task of an object
        ack_handel_t *ack = (ack_handel_t *) handle;
        static_cast<Com *>(ack->_this)->acknowledge_handler_task(*ack->transmitpackage);
    }

    void Com::transmissionQueueHandeler(void *_this) {
//    static overload to enable RTOS task of an object
        static_cast<Com *>(_this)->transmissionQueueHandeler();
    }


    Com::err_t Com::incoming_connection(uint8_t *package, uint8_t package_size) {
/*
 *    TODO O(N) is eigenlijk te langzaam.
 *    Als we het meteen kunnen opzoeken door ook nog een set te maken (of hashing table) van de namen
 *    scheelt dat ons heel veel tijd want dan is het O(1)!
 */
        uint8_t ack[2] = {0x02};
        transmitpackage_t tp;
        if (get_status() != status_t::RUNNING) return err_t::NOT_STARTED;
        ESP_LOGV(LOG_TAG, "incoming connection");
        if (package[0] == 2 && package_size == 2) {
//		recieved ack
            if (times_tried[package[1]] > 0)
                xTaskNotifyGive(ack_tasks[package[1]]);
            return err_t::OK;
        }

        if (!transmitpackage_t::binary_to_transmitpackage(package, tp))
            return err_t::INVALID_PARAMETERS;
        if (package[1] > 0) {
//		send ack
            ack[1] = package[1];

            this->getDriver()->transmit(ack, 2);
        }
        for (const auto &channel : channels) {
            if (channel.port == tp.to_port) {
                xQueueSend(*channel.handeler, &tp, 0);
                return err_t::OK;
            }
        }
        return err_t::NO_CHANNEL;

    }

    Com::err_t Com::register_candidate_driver(ComDriver *driver) {
//    comdriver moet eigenlijk een referentie zijn en niet een levend object.

        ESP_LOGD(LOG_TAG, "registering driver: %s", driver->getName());
        if (driverCandidates.find(driver) != driverCandidates.end()) {
            return err_t::DRIVER_EXISTS;
        }
        if (!driverCandidates.insert(driver).second)
            return err_t::BUFFER_OVERFLOW;
//    select new driver if com is running
        if (get_status() == status_t::RUNNING) {
//        TODO its better to rate this new driver and compare its score to the current driver.
            selectDriverTask();
        }
        return err_t::OK;
    }


    Com::err_t Com::unregister_candidate_driver(ComDriver *driver) {
        if (driverCandidates.erase(driver)) {
//        if we delete our own driver, search for a new one
            if (this->driver == driver)
                selectDriverTask();
            return err_t::OK;
        } else
            return err_t::NO_DRIVER;
    }

    void Com::_selectDriverTask() {
        auto dr = pickDriver();
        if (std::get<1>(dr) == err_t::OK) {
            setDriver(*std::get<0>(dr));
        } else {
            ESP_LOGE(LOG_TAG, "Failed to pick new driver: %d", (uint8_t) std::get<1>(dr));
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

    Com::err_t Com::transmit(transmitpackage_t transmitpackage) {
        err_t return_msg;
//	TODO use malloc for data size
        uint8_t data[MAX_DATA_SIZE + transmitpackage_t::HEADER_SIZE];
        char task_name_buf[15];

        if (!transmitpackage.resend && transmitpackage.priority < 2) {
//	get N count if message has high priority and is not already set.
            transmitpackage.n = n_count;
//		if N count package from the prev loop is still in transit
            if (times_tried[n_count] > 0) {
//			wait for it to be done, before sending
                if (ulTaskNotifyTake(pdTRUE, ACK_TIMEOUT * MAX_TRIES) != n_count) {
//					TODO set timer to allow for ack from other N pack in between instead of force quiting
                    vTaskDelete(ack_tasks[n_count]);
                    times_tried[n_count] = 0;
                }
            }
        }
        transmitpackage_t::transmitpackage_to_binary(transmitpackage, data);
        watched_packages[n_count] = transmitpackage;
        return_msg = this->getDriver()->transmit(data, transmitpackage.size, transmitpackage.priority);

//	start ack task if there is not already one and priority is high
        if (!transmitpackage.resend && transmitpackage.priority < 2) {
            ack_handel_t p = {
                    this,
                    &watched_packages[n_count]
            };

            sprintf(task_name_buf, "ack_handler_%d", transmitpackage.n);
//			TODO set correct priority
            if (xTaskCreate(_acknowledge_handler_task, task_name_buf, 2024, (void *) &p, 5,
                            &ack_tasks[transmitpackage.n]) != pdPASS)
                ESP_LOGE(LOG_TAG, "failed to create task: %s", task_name_buf);
            n_count = n_count >= N_COUNT_MAX ? 1 : n_count + 1;
        }
        return return_msg;
    }
}