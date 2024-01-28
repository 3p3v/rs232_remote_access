#pragma once

#include <Base_setter.hpp>

namespace Logic
{
    /// @brief Underlining operations handler
    class Under_setter : public Base_setter
    {
        using Shared_helper_ptr = std::shared_ptr<Shared_helper>;

        /* Shared objects */
        Shared_helper_ptr helper;

    public:
        /// @brief Send ACK to acknowlage all packets with ID <= id
        /// @param id
        void ack(char id);

        /// @brief Ask to resend all messages >= id
        /// @param id
        void ask_to_resend(char id);

        Under_setter(Shared_helper_ptr &&helper);
        Under_setter(const Shared_helper_ptr &helper);
    };
}