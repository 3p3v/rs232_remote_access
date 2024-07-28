#pragma once

#include <Packet_sett.hpp>
#include <Packet_defs.hpp>

using namespace Logic;

/// @brief Singelton class, ready to apply to Exchanger extension
class Packet_sett_final : public Packet_sett<Packet_defs::Val_t>
{
    using Packet_sett<Packet_defs::Val_t>::Packet_sett;

public:
    /// @brief Get reference to packet settings
    /// @return
    static const Packet_sett_final &get();
};
