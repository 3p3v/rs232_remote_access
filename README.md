# RS-232 remote access

RS-232 remote port access system allows a remote computer user to take control of the RS232 port of dedicated embedded devices equipped with GSM modules.
The solution can be beneficial when key devices cannot be accessed via the local network or when devices need to be diagnosed remotely.

The system consists of three main components: 
- a client application for a remote user,
- a device with an RS232 port, with ability to communicate through GPRS network, 
- a server. 

Communication between the device and the application is implemented via server, using the MQTTv5 standard, encrypted using TLSv1.3.

### Project structure

The repository consists of three branches, one for each project component:
- "client_application",
- "remote_device",
- "server".

## Client application

The application is the main interface through which users can connect to and use the devices.
The application allows users to send and download data from/to the device and to remotely change the device's console port settings. 
When the application is launched, it connects to a user-selected console port available locally on the device.

### Libraries

The application was written using C++17 and utilizes the following libraries:
- STL,
- Boost.Asio,
- Eclipse Paho MQTT C++ Client Library.

Tests were written using GTest framework.

## Device

Remote-controlled embedded device. 
Its main components are:
- RS232 port which can be connected to any local appliance that supports the standard,
- GSM module - used to communicate through GPRS (2G) network.

### Software

The device is written in C language with ESP-IDF framework, which is recommended by the ESP32 chip manufacturer.
It is heavily based on the FreeRTOS real time system.
Libraries and main utilities used:
- ESP-IDF built-in functions,
- FreeRTOS,
- Mbed-TLS,
- Paho MQTT C/C++ Embedded.

### Hardware

The device is based on ESP32 chip with built-in FreeRTOS, which is the main unit controlling the other modules of the device. 
Data conversion from the TTL standard to RS232 is performed by the MAX3232 chip. 
Communication with the server is made possible by using the proven GSM/GPRS module SIM800l.

## Server

The server uses three applications: reverse proxy, MQTT broker and database. 

The whole system is based on Docker technology, so deploying the server can be done simply and quickly.
OpenResty, an NGINX-based server that allows LUA scripts, was used as a reverse proxy. 
The MQTT broker used is MOSQUITTO with the mosquitto-go-auth plugin. 
Both previously mentioned components use a MySQL database to store information on system users and devices.


