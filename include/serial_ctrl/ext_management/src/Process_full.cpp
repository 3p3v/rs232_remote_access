#include <Process_full.hpp>

namespace Logic
{   
    void Process_full::add_ext(Shared_ext &&ext_ptr)
    {           
        auto &ext = *ext_ptr;
        
        /* Add extension ptr */
        if (exts.insert(std::move(ext_ptr)).second == false)
            throw std::logic_error{"Extension of this type is already in use!"};
        
        /* Commands for communcation between extension and device */
        auto cmds = ext.get_cmds();
        std::for_each(
            cmds.begin(),
            cmds.end(),
            [this](auto &cmd)
            {
                cmd_storage.add_cmd(std::move(cmd));
            });

        /* Commands for communcation between extensions */
        ext_storage.add_worker(ext);
    }

    Process_full::Ext_cont::Elem_t::Id_t Process_full::Ext_cont::Hash::operator()(const Elem_t &ext) const noexcept
    {
        return ext.id;
    }

    bool Process_full::Ext_cont::KeyEqual::operator()(const Elem_t &ext1, const Elem_t &ext2) const noexcept
    {
        return ext1.id == ext2.id;
    }
}