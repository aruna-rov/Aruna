//
// Created by noeel on 17-08-19.
//

#ifndef ARUNA_COMMTYPES_H
#define ARUNA_COMMTYPES_H

#include <stdlib.h>
#include <string.h>
#include <queue>

namespace aruna {
	namespace comm {

		// kind of link, wired, wireless of non existing
		enum class link_t {
			RADIO,
			WIRED,
			NONE
		};

		// status of the comm object
		enum class status_t {
			RUNNING,
			STOPPED,
			PAUSED
		};

		typedef uint8_t port_t;

		// comm data size minus the header
		// TODO increasing this causes an overflow
		static constexpr size_t MAX_DATA_SIZE = 150;

		/**
		 * transmit ready package.
		 */
		struct transmitpackage_t {
			constexpr static uint HEADER_SIZE = 4;
			/**
			 * @brief  channel who is sending the data.
			 */
			port_t from_port;

			/**
			* to whom to send it to.
			*/
			port_t to_port;

			/*
			 * Number of the package (used for ack.)
			 */
			uint8_t n;


			/**
			 * @brief  Data to be transmitted.
			 */
			uint8_t *data_transmitting;

			/**
			 * pointer to where incoming data must be stored.
			 */
			uint8_t *data_received;

			/**
			 * size of the data
			 */
			uint8_t data_lenght;

			/**
			 * total size of package (+ the size of the size variable)
			 */
			uint8_t size;


			/**
			 * notify the task on ack/no ack
			 */
			bool notify_on_ack = false;

			/**
			 * is the package being resend?
			 */
			bool resend = false;


			/**
			 * Get binary array of transmitpackage, for sending over a link.
			 * bin arrangement: size, n, from_port, to_port, data
			 * @param transp package to make a binary from.
			 * @param bin to store the data to.
			 */
			static void transmitpackage_to_binary(transmitpackage_t transp, uint8_t *bin) {

				//		bit 0: size
				memcpy(&bin[0],
					   &transp.size,
					   (sizeof(transp.size)));
				//		bit 1: N
				memcpy(&bin[sizeof(transp.size)],
					   &transp.n,
					   (sizeof(transp.n)));
				//		bit 2: from port
				memcpy(&bin[sizeof(transp.size) + sizeof(transp.n)],
					   &transp.from_port,
					   (sizeof(transp.from_port)));
				//		bit 3: to port
				memcpy(&bin[sizeof(transp.size) + sizeof(transp.n) + sizeof(transp.from_port)],
					   &transp.to_port,
					   sizeof(transp.to_port));
				//		bit 4: data
				memcpy(&bin[sizeof(transp.size) + sizeof(transp.n) + sizeof(transp.from_port) + sizeof(transp.to_port)],
					   transp.data_transmitting,
					   transp.data_lenght);
			}

			/**
			 * get tansmitpackage of binary array.
			 * @param bin array that stores the info
			 * @param transp transmitpackage that the data need to go to.
			 * @return true if succeeded, false if not
			 */
			static bool binary_to_transmitpackage(uint8_t *bin, transmitpackage_t &transp) {
				//		check for complete header
				const static int header_length = (sizeof(transp.size) + sizeof(transp.n) + sizeof(transp.from_port) +
												  sizeof(transp.to_port));
				if (bin[0] < header_length)
					return 0;

				int dataLength = bin[0] - header_length;
				transp.data_received = (uint8_t *) malloc(dataLength);
				memcpy(&transp.size, &bin[0], (sizeof(transp.size)));
				memcpy(&transp.n, &bin[sizeof(transp.size)], (sizeof(transp.n)));
				memcpy(&transp.from_port, &bin[sizeof(transp.size) + sizeof(transp.n)], (sizeof(transp.from_port)));
				memcpy(&transp.to_port, &bin[sizeof(transp.size) + sizeof(transp.n) + sizeof(transp.from_port)],
					   (sizeof(transp.to_port)));
				memcpy(transp.data_received,
					   &bin[sizeof(transp.size) + sizeof(transp.n) + sizeof(transp.from_port) + sizeof(transp.to_port)],
					   dataLength);
				transp.data_lenght = dataLength;
				return 1;
			}
		};

		/**
		 * endpoint type of a comm channel
		 */
		struct channel_t {
			pthread_cond_t in_buffer_not_empty;
			pthread_mutex_t in_buffer_lock;
			std::queue<transmitpackage_t> in_buffer;
			err_t register_err = err_t::NOT_STARTED;

			channel_t(port_t port);
			~channel_t();

			err_t send(port_t to_port, uint8_t *data, size_t data_size, bool wait_for_ack = false);

			/**
			 * @brief port nr. of the endpoint
			 */
			const port_t port;

			/**
			 * @brief handeler to handle incomming connections
			 * @note incomming data is not garanteed to be `DATA_SIZE` in length. Could be smaller.
			 */
//            QueueHandle_t *handeler;
			bool receive(transmitpackage_t *tpp) {
				pthread_mutex_lock(&in_buffer_lock);
				while (in_buffer.empty()) {
					pthread_cond_wait(&in_buffer_not_empty, &in_buffer_lock);
				}
				*tpp = in_buffer.front();
				pthread_mutex_unlock(&in_buffer_lock);
				return true;
			}

//            TODO bool receive(transmitpackage_t*, uint32_t timeout_ms)
			bool receive_clear() {
				free(in_buffer.front().data_received);
				in_buffer.pop();
				return true;
			}

			bool operator<(const channel_t &b) const {
				return this->port < b.port;
			}

			bool operator==(const uint8_t port) const {
				return this->port == port;
			}

			struct compare_refrence {
				inline bool operator()(const channel_t* a, const channel_t* b) const {
					return a->port < b->port;
				}
			};

			struct compare_value {
				inline bool operator()(const channel_t& a, const channel_t& b) const {
					return a.port < b.port;
				}
			};
		};
	}
}
#endif //ARUNA_COMMTYPES_H
