#include <Setup_loader.hpp>
#include <iostream>
#include <fstream>
#include <Set_defs.hpp>
#include <Setup_defs.hpp>

namespace Impl
{
    Setup_loader::Setup_loader()
    {
        /* Connection settings */
        console.add_cmd(
            "username",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.user.emplace(Mqtt_port::User::Option::username, std::move(str));
                }));

        console.add_cmd(
            "password",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.user.emplace(Mqtt_port::User::Option::password, std::move(str));
                }));

        // console.add_cmd(
        //     "user_id",
        //     Command::Policies<>::Dyn_handle(
        //         [this](std::string &&str)
        //         {
        //             data_pack.user.emplace(Mqtt_port::User::Option::id, std::move(str));
        //             data_pack.user.emplace(Mqtt_port::User::Option::no_clean, "");
        //         }));

        console.add_cmd(
            "ip",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::ip, std::move(str));
                }});

        console.add_cmd(
            "port",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::port, std::move(str));
                }});

        console.add_cmd(
            "pem",
            // Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::pem, std::move(str));
                }});

        console.add_cmd(
            "cert",
            // Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::cert, std::move(str));
                }});

        /* Device setings */
        console.add_cmd(
            "device",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    auto itr = std::find(
                        data_pack.devices.begin(),
                        data_pack.devices.end(),
                        str);

                    if (itr == data_pack.devices.end())
                    {
                        data_pack.devices.emplace_back<Info_pack>(std::move(str));
                        current_ = data_pack.devices.end() - 1;
                    }
                    else
                    {
                        current_ = itr;
                    }
                }});

        console.add_cmd(
            "baud_rate",
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.baud_rate = Set_defs::baud_rate_trans(str);
                }));

        console.add_cmd(
            "parity",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.parity = Set_defs::parity_trans(str);
                }));

        console.add_cmd(
            "char_size",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.char_size = Set_defs::char_size_trans(str);
                }));

        console.add_cmd(
            "stop_bits",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.stop_bits = Set_defs::stop_bits_trans(str);
                }));

        console.add_cmd(
            "write_access",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.write_access = Setup_defs::write_access_trans(str);
                }));

        console.add_cmd(
            "com",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.info.port = std::move(str);
                    current_device.port_set = true;             // TODO remove once port is set in same line with MAC
                }));

        /* App settings */
        console.add_cmd(
            "debug",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.debug = Setup_defs::bool_trans(str);
                }));

        console.add_cmd(
            "close_on_timeout",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.close_on_timeout = Setup_defs::bool_trans(str);
                }));

        console.add_cmd(
            "close_on_data_loss",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.close_on_data_loss = Setup_defs::bool_trans(str);
                }));

        console.add_cmd(
            "close_on_protocol_error",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.close_on_protocol_error = Setup_defs::bool_trans(str);
                }));
    }

    Setup_loader::Info_pack &Setup_loader::get_current()
    {
        if (current_ == data_pack.devices.end())
            throw std::logic_error{"No device supplied! Cannot continue operation!"};

        return *current_;
    }

    void Setup_loader::load_data_handler(const std::string &file_name)
    {
        std::ifstream s_file;
        s_file.open(file_name);

        if (s_file.is_open())
        {
            std::string line;
            while (std::getline(s_file, line))
            {
                auto itr = std::find(
                    line.cbegin(),
                    line.cend(),
                    '#'
                );

                /* Delete comments */
                if (itr != line.cend())
                {
                    line = std::string{line.cbegin(), itr};
                }
                
                if (line.size())
                    console.exec(line.begin(), line.end());
            }

            s_file.close();

            console.check();
            data_loaded = true;
        }
        else
        {
            throw std::runtime_error(
                "Configuration file: \"" + file_name + "\" not found or could not be opened!");
        }

        /* Check if every device has a specyfied port */
        if (std::none_of(data_pack.devices.cbegin(),    //TODO delete when specyfying port is unskipable
                         data_pack.devices.cend(),
                         [](auto &d)
                         {
                             return d.port_set;
                         }))
        {
            throw std::logic_error{"Some devices do not have specyfied port!"};
        }
    }

    Setup_loader::Data_pack &Setup_loader::load_data(const std::string &file_name) &
    {
        load_data_handler(file_name);
        return data_pack;
    }

    Setup_loader::Data_pack Setup_loader::load_data(const std::string &file_name) &&
    {
        load_data_handler(file_name);
        return std::move(data_pack);
    }

    Setup_loader::Data_pack &Setup_loader::get_data_pack() &
    {
        if (data_loaded)
        {
            return data_pack;
        }
        else
        {
            throw std::logic_error(
                "Tried to get data that was not loaded yet!");
        }
    }

    bool Setup_loader::Info_pack::operator==(const Info_pack &ip)
    {
        return this->info.data_ch == ip.info.data_ch;
    }

    bool Setup_loader::Info_pack::operator==(const std::string &name)
    {
        return this->info.data_ch == Topic_defs::data_ch_trans(name);
    }

    Setup_loader::Info_pack::Info_pack(const std::string &&name)
        : Info_pack_base{Dev_info{std::move(name), ""}}
    {
    }
}