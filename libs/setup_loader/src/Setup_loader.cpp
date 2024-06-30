#include <Setup_loader.hpp>
#include <iostream>
#include <fstream>

namespace Impl
{
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