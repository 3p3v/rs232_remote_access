#include <Collective_general.hpp>
#include <Serial_context.hpp>
#include <Setup_deb.hpp>
#include <Impl_starter.hpp>

/* Startup data */
static const std::string setup = \
    "username admin\n\
password admin\n\
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

        /* Create MQTT client */
        Impl::Impl_starter starter{
            std::move(data_pack.server),
            std::move(data_pack.user)};
        
        /* Connect to broker */
        auto [future, serial_thread] = starter.connect();

        /* Initialize core */
        auto core = ::Impl::Collective_general{
            starter.get_adder(),
            data_pack.app_opts.close_on_timeout,
            data_pack.app_opts.close_on_data_loss,
            data_pack.app_opts.close_on_protocol_error,
            data_pack.app_opts.debug};

        /* Add devices */
        core.add_devces(data_pack.devices.begin(), data_pack.devices.end());

        /* Wait for error in serial thread */
        serial_thread.join();

        if (auto e = future.get().value())
            throw e;

        std::cerr << "Exited without any error." << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
