#include <Setup_deb.hpp>
#include <Collective_general.hpp>
#include <Serial_context.hpp>
#include <string>
/*password admin\n\ */
static const std::string setup = \
    "username admin\n\
ip 127.0.0.1\n\
port 1883\n\
device B0:B2:1C:0B:15:CC\n\
com CNCB0\n\
baud_rate 9600\n\
parity n\n\
debug true";

int main()
{
    try
    {
        /* Get init data from file */
        auto data_pack = Setup_deb{}.load_data(setup);

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
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
