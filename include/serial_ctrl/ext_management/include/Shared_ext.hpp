#include <Remote_ext.hpp>
#include <memory>
#include <Ext_id.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Imitate shared_ptr; add ext id based on type
    class Shared_ext
    {
        using Ext_ptr_t = std::shared_ptr<Remote_ext>;

        Ext_ptr_t ext;

    public:
        using Id_t = unsigned char;

        const Id_t id;

        // template <
        //     typename Ext_t,
        //     std::enable_if_t<std::is_base_of_v<Remote_ext, Ext_t>, bool> = true>
        // Shared_ext(Ext_t &&ext)
        //     : ext{std::make_shared<std::decay_t<Ext_t>>(std::forward<Ext_t>(ext))}, id{Ext_id<Ext_t>::id}
        // {
        // }

        template <
            typename Ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext, Ext_t>, bool> = true>
        Shared_ext(const std::shared_ptr<Ext_t> &ext_ptr)
            : ext{ext_ptr}, id{Ext_id<Ext_t>::id}
        {
        }

        template <
            typename Ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext, Ext_t>, bool> = true>
        Shared_ext(std::shared_ptr<Ext_t> &&ext_ptr)
            : ext{std::move(ext_ptr)}, id{Ext_id<Ext_t>::id}
        {
        }

        // template <
        //     typename Ext_t,
        //     typename... Args_t,
        //     std::enable_if_t<std::is_base_of_v<Remote_ext, Ext_t>, bool> = true>
        // Shared_ext(Args_t &&...args)
        //     : ext{std::make_shared<Ext_t>(std::forward<Args_t>(args)...)}, id{Ext_id<Ext_t>::id}
        // {
        // }

        Remote_ext *operator->() const noexcept;
        Remote_ext &operator*() const noexcept;
        bool operator==(const Shared_ext &ext) const;

        Ext_ptr_t &get_ptr();
    };
}