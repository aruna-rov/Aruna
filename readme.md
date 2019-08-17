# Aruna

Aruna is a library for ROV (remote operated vehicle's) This can be an aircraft, rover, submarine etc. 
The design of the library is very flexable, allowing for multiple diffrent drivers for control of communication, so that it can be used on many diffrent applications. The library's main focus is to create a abstraction layer between the hardware and the application logic.

# Getting started

To get started all you need is an ESP32, anything else is optional. The library is build on esp-idf 3.2.2 and some dependencies as library's. Please see `.gitsubmodules` for an up to date list. These submodules deploy however in the example folder. So you need to install them yourself.

# Modules

This section describes the modules of the library.

## blinky

this is more of a test library, The one thing it does is blink a led on the device.

## comm

This library is responsable for all communication. The library is quite extensive. Diffrent physical layer could be applied using the `Link`.virtual class to allow for diffrent physical implementations

## control

This library handles all the movement of the rov, and is dependand of the `comm` module for communication and uses `Actuator` as a virtual class for drivers.

## log

logging for all the aruna modules

*For more documentation see `./docs`*