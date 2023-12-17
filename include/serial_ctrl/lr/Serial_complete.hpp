#pragma once

#include <memory>
#include <Serial_side.hpp>
#include <Serial_settings.hpp>

namespace Serial_port
{
    namespace Impl
    {
        class Controller;
    }
}

class Serial_complete
{
    const std::string &com;
    std::unique_ptr<Serial_port::Serial> serial{nullptr};

    std::unique_ptr<Serial_side> data_{nullptr};
    std::unique_ptr<Serial_settings> settings_{nullptr};

public:
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    void data_read(const typename Cont_t::const_iterator begin,
                   const typename Cont_t::const_iterator end,
                   Ok_callb &&ok_callb,
                   Ec_callb &&ec_callb);

    Serial_side &data();

    Serial_settings &settings();

    Serial_complete(const std::string &com);
};

template <typename Cont_t, typename Ok_callb, typename Ec_callb>
inline void Serial_complete::data_read(const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
{
    data_ = std::make_unique<Serial_side>(
        com,
        serial,
        begin,
        end,
        std::forward<Ok_callb>(ok_callb),
        std::forward<Ec_callb>(ec_callb));

    setings_ = std::make_unique<Serial_side>(com);
}
