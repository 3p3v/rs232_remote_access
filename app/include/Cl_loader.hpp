#include <Ctrl_console.hpp>
#include <Exec.hpp>
#include <Setup_loader.hpp>
#include <vector>
#include <string>
#include <cassert>

namespace Impl
{
    class Cl_loader final
    {   
        Setup_console sc;

        using Command = Setup_console::Command;
        using Mandatority = Setup_console::Mandatority;

    public:
        class Data_pack
        {
        public:
            Setup_loader::Data_pack config_data;
        };

    private:
        Data_pack data_pack;

        template <typename Itr_t>
        Data_pack load(Itr_t begin, Itr_t end);

        void exec(std::string &str);
        void exec(const char * const str);

    public:
        Data_pack load(std::vector<std::string> &&argv) &&;
        Data_pack load(unsigned int argc, const char * const * const argv) &&;

        Cl_loader();
        Cl_loader(Cl_loader &&) = delete;
        Cl_loader& operator=(Cl_loader &&) = delete;
        Cl_loader(const Cl_loader &) = delete;
        Cl_loader& operator=(const Cl_loader &) = delete;
        ~Cl_loader();
    };

    template <typename Itr_t>
    inline Cl_loader::Data_pack Cl_loader::load(Itr_t begin, Itr_t end)
    {   
        begin = begin + 1;

        assert((end - begin) >= 0 && "Cannot have less than 1 command line argument!");
        
        std::for_each(
            begin,
            end,
            [this](auto &str)
            {
                exec(str);
            }
        );
        
        return std::move(data_pack);
    }
}