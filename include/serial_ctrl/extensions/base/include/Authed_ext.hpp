// #pragma once

// #include <Remote_ext.hpp>

// using namespace Job_ctrl;

// namespace Logic
// {
//     class Process_manager;

//     /// @brief Extension with ability to add other modules
//     class Authed_ext : public Remote_ext
//     {
//     public:
//         using Manager_ptr = std::weak_ptr<Process_manager>;

//     protected:
//         Manager_ptr manager;

//     public:
//         /// @brief Reset all extensions
//         void reset_exts_job();

//         template <
//             typename Forwarder_ptr_t,
//             typename Manager_ptr_t>
//         Authed_ext(
//             Forwarder_ptr_t &&forwarder,
//             Manager_ptr_t &&manager,
//             Notyfier &&notyfier)
//             : Remote_ext{std::forward<Forwarder_ptr_t>(forwarder), std::move(notyfier)},
//               manager{std::forward<Manager_ptr_t>(manager)}
//         {
//         }

//         template <
//             template <typename> class Process_manager_ptr_t,
//             typename Process_manager_t,
//             std::enable_if_t<
//                 std::is_base_of_v<
//                     Process_manager,
//                     Process_manager_t>,
//                 bool> = true>
//         Authed_ext(
//             Process_manager_ptr_t<Process_manager_t> &&process_manager,
//             Notyfier &&notyfier)
//             : Authed_ext{process_manager, process_manager, std::move(notyfier)}
//         {
//         }

//         template <
//             template <typename> class Process_manager_ptr_t,
//             typename Process_manager_t,
//             std::enable_if_t<
//                 std::is_base_of_v<
//                     Process_manager,
//                     Process_manager_t>,
//                 bool> = true>
//         Authed_ext(
//             const Process_manager_ptr_t<Process_manager_t> &process_manager,
//             Notyfier &&notyfier)
//             : Authed_ext{process_manager, process_manager, std::move(notyfier)}
//         {
//         }

//         Authed_ext(const Authed_ext &) = delete;
//         Authed_ext &operator=(const Authed_ext &) = delete;
//         Authed_ext(Authed_ext &&) = default;
//         Authed_ext &operator=(Authed_ext &&) = default;
//         ~Authed_ext() = 0;
//     };
// }