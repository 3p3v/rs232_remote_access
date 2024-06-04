#include <Info_pack_base.hpp>


namespace Impl
{
    Info_pack_base::Info_pack_base(Dev_info &&info)
        : info{std::move(info)}
    {
    }
}