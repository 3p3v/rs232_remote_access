#include <Cl_loader.hpp>
#include <algorithm>
#include <Setup_loader.hpp>
#include <cassert>
#include <cstring>

void Impl::Cl_loader::exec(std::string &str)
{
    sc.exec(str.begin(), str.end());
}

void Impl::Cl_loader::exec(const char *const str)
{
    sc.exec(str, str + std::strlen(str));
}

Impl::Cl_loader::Data_pack Impl::Cl_loader::load(std::vector<std::string> &&argv) &&
{
    return load(argv.begin(), argv.end());
}

Impl::Cl_loader::Data_pack Impl::Cl_loader::load(unsigned int argc, const char * const * const argv) &&
{    
    return load(argv, argv + argc);
}

Impl::Cl_loader::Cl_loader()
{
    sc.add_cmd(
        "-conf",
        Mandatoriness::Mandatory,
        Command::Policies<>::Dyn_handle(
            [this](std::string &&arg)
            {
                /* Get init data from file */
                data_pack.config_data = ::Impl::Setup_loader{}.load_data(arg);
            }));
}

Impl::Cl_loader::~Cl_loader() = default;