#include <Remote_ext.hpp>
#include <memory>
#include <Ext_id.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Imitate shared_ptr; add ext id based on type
    class Shared_ext
    {
        std::unique_ptr<Remote_ext> ext;

    public:
        using Id_t = unsigned char;

        const Id_t id;

        template <
            typename Remote_ext_t,
            typename = std::enable_if_t<std::is_base_of_v<Remote_ext, std::decay_t<Remote_ext_t>>>>
        Shared_ext(Remote_ext_t &&ext);

        Remote_ext *operator->() const noexcept;
        Remote_ext &operator*() const noexcept;
        bool operator==(const Shared_ext &ext) const noexcept;
    };

    template <typename Remote_ext_t, typename>
    Shared_ext::Shared_ext(Remote_ext_t &&ext)
        : ext{std::make_unique<std::decay_t<Remote_ext_t>>(std::forward<Remote_ext_t>(ext))},
          id{Ext_id<std::decay_t<Remote_ext_t>>::id}
    {
    }
}