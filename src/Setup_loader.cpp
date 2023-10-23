#include <Setup_loader.hpp>
#include <iostream>
#include <fstream>

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