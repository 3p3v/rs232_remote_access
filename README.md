# RS-232 remote access - Client application
This branch contains client application part of the project.

The application is the main interface through which users can connect to and use the devices.
It allows users to send and download data from/to the device and to remotely change the device's console port settings. 
Additionally, user can forward data from the devices by connecting the app to the specified local serial ports.
The app uses communicates asynchronously with both MQTT broker and local serial port.

The specification of the protocol created especially for this project can be found in [resources directory](./resources/).

## Libraries
The application was written using C++17 and utilizes the following libraries:
- STL,
- [Boost](https://www.boost.org/) - version 1.83 was used,
- [Boost.Asio](https://www.boost.org/doc/libs/1_83_0/doc/html/boost_asio.html) - used for asynchronous serial port operations (version 1.83 was used),
- [Eclipse Paho MQTT C++ Client Library](https://github.com/3p3v/paho.mqtt.cpp.unique) - fork of Paho MQTT C++ library (1.20) (asynchronous MQTT client), which uses smart pointers instead of raw pointers. [Eclipse Paho MQTT C Client Library](https://github.com/eclipse/paho.mqtt.c) is needed to run the library. Version 1.3.13 was used.

Tests were written using GTest framework.

## Project structure
The main functions can be found in [app](./app/) for final app and [app_debug](./app_debug/) - for debug purposes.

The rest of the files are located in [libs](./libs/) directory:
- [core](./libs/core/) contains files that implement the connections between user and the remote device in respect to the remote's protocol,
    - the client's side implementation of the protocol can be found in [extensions directory](./libs/core/core_agn/extensions/),
- [main_modules](./libs/main_modules/) implements the management of the devices, error handling etc.,
- [setup_loader](./libs/setup_loader/) implements loading structures from setup file.

Core and main_modules were split into *_agn and *_impl.

Classes in *_agn are implementation-agnostic and thanks to that they were able to be unit-tested.
These classes do not depend on any information about low-level protocols.

Classes in *_impl depend on the low-level implementation of the device:
- the majority of classes are only dependent on information that would be needed for any implementation using MQTT and RS-232 protocols,
- only few classes depend on low-level driver API (i.e. Boost.Asio and Paho).

## Building the project
To build the project, the libraries specified above are needed.
It is required to set correct paths to them in the [main CMakeLists.txt file](./CMakeLists.txt).

## Config file
While starting the app, user is obligated to pass a path to config file using "-conf" parmaeter.

The config file commands are listed below:
| Command                   | Possible arguments | Default| Mandatoriness | Explanation |
|---------------------------|--------------------|--------|---------------|-------------|
| username                  | NA                 | NA     | true          | Username used to log in to MQTT broker |
| password                  | NA                 | NA     | false         | Password used to log in to MQTT broker |
| ip                        | NA                 | NA     | true          | IP of the server |
| port                      | NA                 | NA     | true          | Port of the server |
| pem                       | NA                 | NA     | false         | Path to user pem file generated by the server |
| cert                      | NA                 | NA     | false         | Path to server certificate |
| device                    | NA                 | NA     | true          | Remote device name (MAC); must be defined at least once |
| baud_rate                 | 9600-115200        | 9600   | false         | Baud rate of remote devices serial port |
| parity                    | n/n/o              | e      | false         | Parity of remote devices serial port (even/none/odd) |
| char_size                 | 5-8                | 8      | false         | Char size rate of remote devices serial port |
| stop_bits                 | o/p/t              | o      | false         | Stop bits of remote devices serial port (one/one point five/two) |
| write_access              | NA                 | true   | false         | If user has privileges to devices data topic |
| com                       | NA                 | NA     | true          | Serial name of a local port to connect to |
| debug                     | NA                 | false  | false         | enable debug messages (output in console) |
| close_on_timeout          | NA                 | true   | false         | Close a connection if the corresponding device did not respond in time |
| close_on_data_loss        | NA                 | true   | false         | Close a connection if the corresponding device could not resend the missing data |
| close_on_protocol_error   | NA                 | true   | false         | Close a connection if the corresponding device sent unknown command |
