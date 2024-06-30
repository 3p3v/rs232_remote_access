#include <Collective_general.hpp>
#include <Serial_context.hpp>
#include <Cl_loader.hpp>
#include <memory>

int main(unsigned int argc, char **argv)
{
    try
    {
        /* Load init data */
        auto data_pack = std::make_unique<::Impl::Setup_loader::Data_pack>(::Impl::Cl_loader{}.load(argc, argv).config_data);

        /* Initialize core */
        auto core = ::Impl::Collective_general{
            std::move(data_pack->server),
            std::move(data_pack->user),
            data_pack->app_opts.close_on_timeout,
            data_pack->app_opts.close_on_data_loss,
            data_pack->app_opts.close_on_protocol_error,
            data_pack->app_opts.debug};

        /* Add devices */
        core.add_devces(data_pack->devices.begin(), data_pack->devices.end());

        data_pack.release();

        /* Run underlying Boost context for serial ports */
        Phy_serial::Serial_context::run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
