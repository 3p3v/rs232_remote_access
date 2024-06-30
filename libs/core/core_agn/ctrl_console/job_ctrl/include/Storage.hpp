#pragma once

#include <Ws_ext.hpp>

namespace Job_ctrl
{
    /// @brief Interface for adding workers
    class Storage : public Ws_ext
    {
    public:
        /// @brief Add new worker
        /// @param worker
        void add_worker(Worker &worker);

        /// @brief Remove worker
        /// @param worker
        void remove_worker(const Worker &worker);

        using Ws_ext::Ws_ext;
    };
}