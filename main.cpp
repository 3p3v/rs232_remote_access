#include <iostream>
#include <Local_serial.hpp>
// #include <Traffic_mqtt_local.hpp>
#include <Mqtt_controller.hpp>

class Exec_console : public Mqtt_port::Executor
{
public:
    void exec(const Data &data);
};

class My_callbacks : public Mqtt_port::Callbacks
{
public:
    /* Callbacks */
    void write_call(const std::string &channel_name, std::size_t write_len)  {std::cout << "ok\n";};
    void read_call(const Mqtt_port::Data &begin, std::size_t write_len)  {};
    void write_err_call(const int code)  {};
    void read_err_call(const int code)  {};
    void conn_call()  {};
    void conn_err_call()  {};
};


// template <typename Str, typename Write_handle_t, typename Conn_handle_t>
// decltype(auto) make_mqtt_controller_(Str &&server_address,
//                                      Mqtt_port::Callbacks &cb,
//                                      std::shared_ptr<Mqtt_port::Validator> validator,
//                                      Write_handle_t write_handle,
//                                      Conn_handle_t conn_handle,
//                                      std::shared_ptr<mqtt::async_client> client)
// {
//     return Mqtt_port::Mqtt_controller<Write_handle_t,
//                                       Conn_handle_t>(server_address,
//                                                      std::move(cb),
//                                                      validator,
//                                                      write_handle,
//                                                      conn_handle,
//                                                      client);
// }

// template <typename Str>
// decltype(auto) make_mqtt_controller(std::string &&server_address, Mqtt_port::Callbacks &cb)
// {
//     auto client = std::make_shared<mqtt::async_client>(server_address, "1");
//     auto validator = std::make_shared<Mqtt_port::Validator>();

//     auto write_handle = [client](const std::string &channel_name, const std::vector<unsigned char> &data, std::size_t write_len)
//     { /*client.get()->publish(channel_name, data, write_len, 0, false); */};
//     auto conn_handle = [client](const std::string &channel)
//     { /*client.get()->subscribe(channel, 0, nullptr, nullptr);*/ };

//     return make_mqtt_controller_(server_address,
//                                  std::move(cb),
//                                  validator,
//                                  write_handle,
//                                  conn_handle,
//                                  client);
// }

template <typename Call_t>
decltype(auto) make_mqtt_controller(std::string &&server_address)
{
    return Mqtt_port::Mqtt_controller<My_callbacks>(server_address);
}


int main(int, char **)
{
    std::cout << "Hello, from mqtt_client!\n";

    try
    {
        auto validator = std::make_shared<Mqtt_port::Validator>();
        auto a = validator;
        // auto callbacks = My_callbacks{};
        auto controller = Mqtt_port::Mqtt_controller("127.0.0.1:1883", std::make_unique<My_callbacks>());
        controller.run();
        // Mqtt_port::Traffic_mqtt_controller ctrl{"127.0.0.1:1883"};
        // ctrl.add_channel("console_device1", Mqtt_port::Executor{});
    }
    catch (std::exception &)
    {
    }
}
