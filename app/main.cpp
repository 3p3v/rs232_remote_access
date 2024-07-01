#include <Collective_general.hpp>
#include <Serial_context.hpp>
#include <Cl_loader.hpp>
#include <memory>
#include <Impl_starter.hpp>

int main(unsigned int argc, char **argv)
{
    try
    {
        /* Load init data */
        auto data_pack = std::make_unique<::Impl::Setup_loader::Data_pack>(::Impl::Cl_loader{}.load(argc, argv).config_data);

        /* Create MQTT client */
        Impl::Impl_starter starter{
            std::move(data_pack->server),
            std::move(data_pack->user)};
        
        /* Release loaded data */
        data_pack.release();
        
        /* Connect to broker */
        auto [future, serial_thread] = starter.connect();

        /* Initialize core */
        auto core = ::Impl::Collective_general{
            starter.get_adder(),
            data_pack->app_opts.close_on_timeout,
            data_pack->app_opts.close_on_data_loss,
            data_pack->app_opts.close_on_protocol_error,
            data_pack->app_opts.debug};

        /* Add devices */
        core.add_devces(data_pack->devices.begin(), data_pack->devices.end());

        /* Wait for error in serial thread */
        serial_thread.join();

        if (future.get().has_value())
            throw future.get().value();

        std::cerr << "Exited without any error." << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
