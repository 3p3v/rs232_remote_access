#include <Process_manager.hpp>

namespace Logic
{
    void Process_manager::add_ext(const Shared_ext &ext_ptr)
    {
        add_ext(Shared_ext(ext_ptr));
    }
    
    void Process_manager::add_ext(Shared_ext &&ext_ptr)
    {           
        /* Add extension ptr */
        if (exts.insert(ext_ptr).second == false)
            throw std::logic_error{"Extension of this type is already in use!"};
        
        /* Commands for communcation between extension and device */
        auto cmds = ext_ptr->get_cmds();
        std::for_each(
            cmds.begin(),
            cmds.end(),
            [this](Ctrl_cmd_pair &cmd)
            {
                add_cmd(std::move(cmd));
            });

        /* Commands for communcation between extensions */
        add_worker(std::move(ext_ptr.get_ptr()));
    }

    Process_manager::~Process_manager() = default;

    Process_manager::Ext_cont::Elem_t::Id_t Process_manager::Ext_cont::Hash::operator()(const Elem_t &ext) const noexcept
    {
        return ext.id;
    }

    bool Process_manager::Ext_cont::KeyEqual::operator()(const Elem_t &ext1, const Elem_t &ext2) const noexcept
    {
        return ext1.id == ext2.id;
    }
}