#include <Setup_loader.hpp>
#include <iostream>
#include <fstream>

Setup_loader::Device_iter &Setup_loader::get_current()
{
    if (current_ == data_pack.devices.end())
        throw Exception::Setup_except{"No device supplied! Can not continue operation!"};

    return current_;
}

void Setup_loader::load_data_handler()
{
    std::ifstream s_file;
    s_file.open(file_name);

    if (s_file.is_open())
    {
        std::string line;
        while (std::getline(s_file, line))
        {
            console.exec(line.begin(), line.end());
        }

        s_file.close();

        console.check();
        data_loaded = true;
    }
    else
    {
        throw std::runtime_error("Configuration file: " + file_name + "not found or could not be opened!");
    }

    /* Check if every device has a specyfied port */
    if (std::none_of(data_pack.devices.cbegin(), 
                     data_pack.devices.cend(),
                     [](const auto &d) {
                        return d.second.port_set;
                     }))
    {
        throw Exception::Setup_except{"Some devices do not have specyfied port!"};
    }
}

Setup_loader::Data_pack& Setup_loader::load_data() &
{
    load_data_handler();
    return data_pack;
}

Setup_loader::Data_pack Setup_loader::load_data() &&
{
    load_data_handler();
    return std::move(data_pack);
}

Setup_loader::Data_pack& Setup_loader::get_data_pack() &
{
    if (data_loaded)
    {
        return data_pack;
    }
    else
    {
        throw std::logic_error("Tried to get data that was not loaded yet!");
    }
}