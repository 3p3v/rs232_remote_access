#pragma once

#include <Job.hpp>
#include <Notyfier_defs.hpp>
#include <stdexcept>
#include <memory>

using namespace Job_ctrl;

namespace Logic
{
    class Error_job : public Job_identity<Error_job>
    {
        /// @brief Have to be unique_ptr, otherwise would have to use reference - that would be unsafe
        std::unique_ptr<std::exception> exc;

    public:
        using Dev_num = Notyfier_defs::Dev_num;

        Dev_num num;

        const std::exception &get_exc() const noexcept;

        template <
            typename Exc_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    std::exception,
                    Exc_t>>>
        Error_job(Dev_num num, const Exc_t &exc);
        Error_job(Error_job &&) = default;
        Error_job& operator=(Error_job &&) = default;
        Error_job(const Error_job &);
        Error_job& operator=(const Error_job &);
        ~Error_job() = default;
    };

    template <
            typename Exc_t,
            typename>
    Logic::Error_job::Error_job(Dev_num num, const Exc_t &exc)
        : num{num}, exc{std::make_unique<Exc_t>(exc)}
    {
    }
}