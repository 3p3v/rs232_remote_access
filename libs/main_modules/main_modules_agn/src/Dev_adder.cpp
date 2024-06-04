#include <Dev_adder.hpp>
#include <Dev_cont.hpp>


Logic::Dev_adder::Dev_adder(Dev_cont & devs, Worker_storage & notyfier)
    : Dev_ext{devs}, notyfier{notyfier}
{
}