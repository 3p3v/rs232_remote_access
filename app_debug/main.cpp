#include <Setup_loader.hpp>
#include <Collective_general.hpp>
#include <Serial_context.hpp>

static std::string file_name{"../../app_debug/resources/test_setup.conf"};

int main()
{
    /* Get init data from file */
    auto data_pack = ::Impl::Setup_loader{}.load_data(file_name);

    /* Initialize core module and add devices */
    auto core = ::Impl::Collective_general{
        std::move(data_pack.server),
        std::move(data_pack.user),
        data_pack.app_opts.close_on_timeout,
        data_pack.app_opts.close_on_data_loss,
        data_pack.app_opts.close_on_protocol_error,
        data_pack.app_opts.debug};

    core.add_devces(data_pack.devices.begin(), data_pack.devices.end());

    /* Run underlying Boost context for serial ports */
    Phy_serial::Serial_context::run();
}
