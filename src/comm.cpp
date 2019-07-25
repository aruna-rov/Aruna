//
// Created by noeel on 7-1-19.
//

#include "aruna/comm.h"
#include "aruna/log.h"
#include <math.h>
#include <aruna/drivers/comm/CommDriver.h>
#include <pthread.h>
#include <queue>
#include <set>
namespace aruna {
    namespace comm {
        using namespace drivers::comm;

//        private
        namespace {
//        begin ahead declaration
        // variables


        static log::channel_t *log;

        pthread_t transmissionQueueHandeler_thread_handeler;
        pthread_cond_t out_buffer_not_empty;
        pthread_mutex_t out_buffer_critical;
        std::queue<transmitpackage_t> out_buffer;
        std::set<drivers::comm::CommDriver *> driverCandidates;

        /**
         * @brief all endpoints
         */
        std::set<channel_t*> channels;

        /**
         * @brief stores the comm status
         */
        status_t status = status_t::STOPPED;

        /**
         * stores the driver.
         */
        CommDriver *driver;

//        functions

        /**
         * @brief set the comm status
         * @param status: new status
         */
        void set_status(status_t status);

        /**
         * Transmit a package
         * @param transmitpackage package to be transmitted
         * @return err_t.
         * 		-	OK if success
         * 		-	HARDWARE_ERR if the hardware layer failed
         */
        err_t transmit(transmitpackage_t transmitpackage);

        /**
         * @brief  Tramsmission handeler. Do not call directly, blocks CPU.
         * @retval None
         */
        void * transmissionQueueHandeler(void *);

        /**
         * get the driver
         * @return ComdDriver object
         */
        CommDriver *getDriver();

        /**
         * pick the best available driver
         * @return tuple    0: ComDriver best candidate object.
         *                  1: err_t.
         * 1: `NO_DRIVER` if no driver can be found
         * 1: `OK` if all is well
         */
        std::tuple<drivers::comm::CommDriver *, err_t> pickDriver();

        /**
         * set the driver
         * @param driver to use.
         */
        void setDriver(drivers::comm::CommDriver &driver);

        /**
         * rate the driver on speed, errors, active connection, realtime, connection type etc.
         * @param driver
         * @return rating of the driver. Higher is better.
         */
        unsigned int rateDriver(drivers::comm::CommDriver &driver);

        /**
         * pick a new best driver, dont call directly will delete your process.
         */
        void * _selectDriverTask(void *);

        /**
         * start a task to select a driver, does not block.
         */
        void selectDriverTask();

//        end ahead declaration

        void set_status(status_t status) {
            comm::status = status;
        }

        err_t transmit(transmitpackage_t transmitpackage) {
            err_t return_msg;
            uint8_t* data = (uint8_t*) malloc(transmitpackage.data_lenght + transmitpackage_t::HEADER_SIZE);

            transmitpackage_t::transmitpackage_to_binary(transmitpackage, data);
            return_msg = getDriver()->transmit(data, transmitpackage.size);

            free(data);
            return return_msg;
        }

        void * transmissionQueueHandeler(void *) {
            transmitpackage_t transpack;
            err_t transmit_msg = err_t::FAIL;
            while(1) {

                pthread_mutex_lock(&out_buffer_critical);
                while(out_buffer.empty()){
                    pthread_cond_wait(&out_buffer_not_empty, &out_buffer_critical);
                }
                transmit_msg = transmit(out_buffer.front());
                if (transmit_msg != err_t::OK) {
                    log->warning("transmit of: %d, to: %d, failed: 0x%X", transpack.from_port, transpack.to_port,
                             (uint8_t) transmit_msg);
                    log->dump(log::level_t::WARNING, transpack.data_transmitting, transpack.data_lenght);
                } else {
                    out_buffer.pop();
                }
                pthread_mutex_unlock(&out_buffer_critical);

            }
        }
        CommDriver *getDriver() {
            return driver;
        }
        void setDriver(CommDriver &driver) {
            comm::driver = &driver;
        }

        std::tuple<CommDriver *, err_t> pickDriver() {
//    bestpick initalisren omdat hij anders een lege terug kan geven.
            CommDriver *bestPick = nullptr;
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

        unsigned int rateDriver(CommDriver &driver) {
            unsigned int score = 0;
            err_t drivstrt = driver.start();
            if (drivstrt != err_t::OK) {
                log->warning("driver:%s, failed to start: %d", driver.getName(), (int) drivstrt);
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
//            TODO is NONE even posible?
                    return score;
            }
            score *= (int) driver.getSpeed() / 100;

            if (driver.isRealTime())
                score *= 6;

            err_t drivstp = driver.stop();
            if (drivstp != err_t::OK) {
                log->warning("driver: %s, failed to stop: %d", driver.getName(), (int) drivstp);
                return score / 1000;
            }
            return score;
        }
        void * _selectDriverTask(void *) {
            auto dr = pickDriver();
            if (std::get<1>(dr) == err_t::OK) {
                setDriver(*std::get<0>(dr));
            } else {
                log->error("Failed to pick new driver: %d", (uint8_t) std::get<1>(dr));
                comm::stop();
            }
            pthread_cancel(pthread_self());
            return nullptr;
        }

        void selectDriverTask() {
            pthread_create(NULL, NULL, _selectDriverTask, NULL);
        }

        bool register_log() {
            static bool registerd = false;
            if (!registerd){
                log = new log::channel_t("comm");
                registerd = true;
            }
            return registerd;
        }
    }




    err_t start() {
//    pick a driver
        auto dr = pickDriver();
        if (std::get<1>(dr) == err_t::OK) {
//        if driver is found start comm with driver as paramter
            return start(std::get<0>(dr));
        }
//    return pickDriver error.
        return std::get<1>(dr);
    }

    err_t start(CommDriver *driver) {
        switch (get_status()) {
            case status_t::RUNNING:
                return err_t::NOT_STOPPED;
            case status_t::PAUSED:
//            TODO is dit slim? kan ongwenst gedrag vertonen als je van driver wilt wisselen.
                return resume();
            default:
                break;
        }
        int err = 0;
//        register com channel
        register_log();
//    set the driver
        err_t driver_err;
        setDriver(*driver);
        driver_err = getDriver()->start();
//        init pthread mutex and cond
        pthread_cond_init(&out_buffer_not_empty, NULL);
        pthread_mutex_init(&out_buffer_critical, NULL);
        if (driver_err != err_t::OK)
            return driver_err;

        err = pthread_create(&transmissionQueueHandeler_thread_handeler, NULL, transmissionQueueHandeler, NULL);
        if (err) {
            log->error("failed to start transmissionQueueHandeler: %i", err);
//        stop when task failes
            driver_err = getDriver()->stop();
            if (driver_err != err_t::OK)
                log->error("Driver failed to stop: %d", (int) driver_err);
//        TODO driver unsetten en destroyen.
            return err_t::TASK_FAILED;
        }
//    set status to running if all succeeded

        set_status(status_t::RUNNING);

        return err_t::OK;
    }

    err_t stop() {
        switch (get_status()) {
            case status_t::STOPPED:
                return err_t::NOT_STARTED;
            default:
                break;
        }
//    stop all
        err_t driver_err;
        driver_err = getDriver()->stop();
        pthread_cond_destroy(&out_buffer_not_empty);
        pthread_mutex_destroy(&out_buffer_critical);
        pthread_cancel(transmissionQueueHandeler_thread_handeler);
        while(!out_buffer.empty())
            out_buffer.pop();
        set_status(status_t::STOPPED);

        if (driver_err != err_t::OK)
            return driver_err;
        return err_t::OK;
    }

    err_t pause() {
        switch (get_status()) {
            case status_t::STOPPED:
            case status_t::PAUSED:
                return err_t::NOT_STARTED;
            default:
                break;
        }
//        TODO suspend transmissionQueueHandeler_thread_handeler
        set_status(status_t::PAUSED);
        return err_t::OK;
    }

    err_t resume() {
        switch (get_status()) {
            case status_t::STOPPED:
                return err_t::NOT_STARTED;
            case status_t::RUNNING:
                return err_t::NOT_PAUSED;
            default:
                break;
        }
//        TODO continue transmissionQueueHandeler_thread_handeler
        set_status(status_t::RUNNING);
        return err_t::OK;
    }

    err_t register_channel(channel_t *channel) {
/*
 * TODO paramters should consist of the paramters of channel_t together with a handeler.
 * handeler can then later be used for sending and accessing Rx array
 */

//    TODO testen of deze check wel werkt.
        if (channels.find(channel) != channels.end())
            return err_t::CHANNEL_EXISTS;
        if (channels.insert(channel).second) {
            return err_t::OK;
        } else
            return err_t::BUFFER_OVERFLOW;
//    TODO handeler fixen.
    }

    err_t unregister_channel(channel_t &channel) {
        if (channels.erase(&channel))
            return err_t::OK;
        else
            return err_t::NO_CHANNEL;
    }

    err_t
    send(channel_t *channel, port_t to_port, uint8_t *data, size_t data_size, bool wait_for_ack) {
        if (get_status() != status_t::RUNNING) return err_t::NOT_STARTED;
        if (channels.find(channel) == channels.end()) {
            return err_t::NO_CHANNEL;
        }
        uint packages = ceil((double) data_size / (double) MAX_DATA_SIZE);
//	how many packages are we waiting for?
        uint8_t n_to_wait_for = 0;
        //        datasize length
        size_t ds_l;
        //        datasize pointer
        size_t ds_p = 0;

        if (!getDriver()->isEndpointConnected())
            return err_t::NO_CONNECTION;

//	TODO check if tp is allocated
        auto tp = (transmitpackage_t *) calloc(packages, sizeof(transmitpackage_t));

//    loop until all data in send
        while (data_size > 0) {
//		static data
            tp[n_to_wait_for].to_port = to_port;
            tp[n_to_wait_for].from_port = channel->port;
            tp[n_to_wait_for].n = 0;
            tp[n_to_wait_for].notify_on_ack = wait_for_ack;

            tp[n_to_wait_for].data_transmitting = &data[ds_p];

            ds_l = (data_size >= MAX_DATA_SIZE) ? MAX_DATA_SIZE : data_size;

            tp[n_to_wait_for].data_lenght = ds_l;
            tp[n_to_wait_for].size = ds_l + transmitpackage_t::HEADER_SIZE;

            ds_p += ds_l;
            data_size -= ds_l;
            log->verbose("sending from: %d to: %d", tp[n_to_wait_for].from_port,
                     tp[n_to_wait_for].to_port);
            log->dump(log::level_t::VERBOSE, tp[n_to_wait_for].data_transmitting, tp[n_to_wait_for].data_lenght);
            pthread_mutex_lock(&out_buffer_critical);
            out_buffer.push(tp[n_to_wait_for]);
            pthread_mutex_unlock(&out_buffer_critical);
            pthread_cond_broadcast(&out_buffer_not_empty);

            n_to_wait_for++;
        }
        free(tp);
        return err_t::OK;
    }

    bool is_connected() {
        return getDriver()->isEndpointConnected();
    }

    status_t get_status() {
        return status;
    }



    link_t get_link_type() {
        return getDriver()->getLinkType();
    }

    void getName(char *buffer) {
        getDriver()->getName(buffer);
    }

    char *getName() {
        return getDriver()->getName();
    }

    err_t get_channels(char *buffer) {
// TODO
        return err_t::OK;
    }

    unsigned int get_speed() {
        return getDriver()->getSpeed();
    }

    err_t incoming_connection(uint8_t *package, uint8_t package_size) {
/*
 *    TODO O(N) is eigenlijk te langzaam.
 *    Als we het meteen kunnen opzoeken door ook nog een set te maken (of hashing table) van de namen
 *    scheelt dat ons heel veel tijd want dan is het O(1)!
 */
        transmitpackage_t tp;
        if (get_status() != status_t::RUNNING) return err_t::NOT_STARTED;
        log->verbose("incoming connection");

        if (!transmitpackage_t::binary_to_transmitpackage(package, tp))
            return err_t::INVALID_PARAMETERS;

        for (auto channel : channels) {
            if (channel->port == tp.to_port) {
                pthread_mutex_lock((pthread_mutex_t*)&channel->in_buffer_lock);
                channel->in_buffer.push(tp);
                pthread_mutex_unlock((pthread_mutex_t*)&channel->in_buffer_lock);
                pthread_cond_broadcast((pthread_cond_t*)&channel->in_buffer_not_empty);
                return err_t::OK;
            }
        }
        return err_t::NO_CHANNEL;

    }

    err_t register_candidate_driver(CommDriver *driver) {
//    comdriver moet eigenlijk een referentie zijn en niet een levend object.
        register_log();
        log->debug("registering driver: %s", driver->getName());
        if (driverCandidates.find(driver) != driverCandidates.end()) {
            return err_t::DRIVER_EXISTS;
        }
        if (!driverCandidates.insert(driver).second)
            return err_t::BUFFER_OVERFLOW;
//    select new driver if comm is running
        if (get_status() == status_t::RUNNING) {
//        TODO its better to rate this new driver and compare its score to the current driver.
            selectDriverTask();
        }
        return err_t::OK;
    }


    err_t unregister_candidate_driver(CommDriver *driver) {
        if (driverCandidates.erase(driver)) {
//        if we delete our own driver, search for a new one
            if (driver == driver)
                selectDriverTask();
            return err_t::OK;
        } else
            return err_t::NO_DRIVER;
    }

}}
