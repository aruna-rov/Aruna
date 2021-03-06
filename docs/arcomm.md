# ARCOMM

This document describes the Aruna comm protocol, further referenced to as ARCOMM.

Document version: 0.0.1

ARCOMM is designed for a 1:1 connection, one node talking to one other node. Flexibility is kept in mind for further expansion (hardware and application layer especially)

## Hardware layer

the hardware in not defined in ARCOMM, the idea is that ARCOMM is a very flexible protocol that allows for many different hardware layers. The default package layout is however defined and needs to be passed on to the process layer as described below. Extra headers may be added if it is necessary for the hardware link.

### packet layout

1 byte | 1 byte | 1 byte | 1 byte | n byte(s) |
--- | --- | --- | --- | --- |
size | N | from_port | to_port | data

* *size:* the total size of the package including the size byte.
* *N:* the number of the package. Used for acknowledgements, starting from 1 to 255. 0 is used for when an package does not need a acknowledgements.
* *from_port:* from who the package is from.
* *to_port:* destination application.
* *data:* data that is being send, maximum length is 255.

## Processing layer

This layer is responsible for all the logic in ARCOMM.

The maximum length of the data in a package is 255. When an application tries to send something bigger then it must be send in parts.

### Channels

ARCOMM communication is done by channels. Each application can register multiple channels and only deregister channels that they have ownership over. A channel consists of :

* *channel handler:* This is the owner of the channel, and is used for transmitting, receiving packages and manipulating the channel properties.
* *port:* port of the channel (0-255). Each port can only be used once until the channel is destroyed.
* *priority:* priority of the package (0-2). Priority 0 is the highest and must be send instantly regardless of the queue of priority 1 or 2. Queue 1 must be send with queue 2 in between 2 packages.

Each channel can communicate with every other channel on the other host. Communication between two channels on the same node is not possible.

### Acknowledgements

Packages with priority 0 or 1 require acknowledgements. Priority 2 does not use acknowledgements. The *N* in the package indicates the number of the package (1-255), 0 is reserved for priority 2. For each package being send the *N* counter must increase by 1, until 255 is reached then loop back to 1.

When sending a package and not receiving a *ACK* package for that *N* in 500ms then the package must be resend until a maximum of 3 tries is reached (including the initial send)

When a package is received with a positive *N* an *ACK* package must be send as soon as possible.

#### ACK package

1 byte | 1 byte
--- | ---
size | N
`0x02` | `0x01` - `0xFF`

* *size:* size is always 2. This is also used to identify a *ACK* package because normal packages are atleast 4 byte long.
* *N:* package number that you are acknowledging.

## Application layer

Below describes the functions that must be accessible from any application using ARCOMM.

* Start/stop ARCOMM on stopping ARCOMM all the queue's and buffers must be cleared.
* get status of ARCOMM (running, stopped, error)
* Get the name of de ComDriver (hardware layer)
* register channel
* deregister channel
* send packages
* receive packages
* get send status (acknowledge, successfully send, ack failed, send failed)
* get the speed of the link
* get link status (connected, hardware missing etc.)
* get type of the link (wire, wireless)
* get all registered channels
* register candidate driver (a driver for the hardware layer)
* deregister candidate driver
