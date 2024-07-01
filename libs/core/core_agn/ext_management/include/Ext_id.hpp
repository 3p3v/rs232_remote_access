#pragma once 

#include <type_traits>
#include <limits>
#include <cassert>

namespace Logic
{   
    class Ext_id_base
    {
    public:
        using Id_t = unsigned char;

        static Id_t id;
    };

    /// @brief Generate unique id for given extension
    /// @tparam Ext_t Extension class type
    template <
        typename Ext_t>
    class Ext_id
    {
        /// @brief Check if given type is enough to contain all exts types
        /// @return 
        static Ext_id_base::Id_t check_overflow();

    public:
        /// @brief Id, different for every extension
        static Ext_id_base::Id_t id;
    };

    template <typename Ext_t>
    inline Ext_id_base::Id_t Ext_id<Ext_t>::check_overflow()
    {
        auto t = Ext_id_base::id;

        assert(Ext_id_base::id++ < std::numeric_limits<Ext_id_base::Id_t>::max() && "Id type too small!");
        
        return t;
    }

    template <typename Ext_t>
    Ext_id_base::Id_t Ext_id<Ext_t>::id{check_overflow()};
}