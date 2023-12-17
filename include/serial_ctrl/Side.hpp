#pragma once

// #include <unordered_map>
// #include <string>
// #include <memory>
// #include <Device.hpp>
#include <Flow.hpp>
// // #include <Controller.hpp>
// #include <Monitor.hpp>
// #include <Mqtt_msg_cont.hpp>
// #include <Ip_master.hpp>
// #include <Shared_helper.hpp>

class Console;

template <typename Shared_helper_t>
class Side
{
protected:
    /* Shared objects */
    std::shared_ptr<Shared_helper_t> shared_helper;

    /* Byte counter */
    Flow flow;                             

public:
    // const Flow get_flow() const;

    Side(std::shared_ptr<Shared_helper_t> shared_helper);
    Side(Side &&) = default;
    Side &operator=(Side &&) = default;
    Side(const Side &) = delete;
    Side &operator=(const Side &) = delete;
    virtual ~Side() = default;
};

template <typename Shared_helper_t>
Side<Shared_helper_t>::Side(std::shared_ptr<Shared_helper_t> shared_helper)
    : shared_helper{std::move(shared_helper)}
{
    
}