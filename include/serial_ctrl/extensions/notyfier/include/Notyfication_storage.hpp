#pragma once

#include <vector>
#include <stdexcept>
#include <Notyfier_defs.hpp>
#include <Notyfication_observer.hpp>
#include <Storage.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Interface for monitor notyfier
    class Notyfication_storage : private Storage
    {
    public:
        /// @brief 
        /// @param ob 
        void add(Notyfication_observer &ob);

        using Storage::Storage;
    };
}