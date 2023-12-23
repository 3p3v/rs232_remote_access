#include <Serial_side.hpp>

namespace Impl
{
    void Serial_side::buf_check(size_t size)
    {
        if (size > max_buf_size)
            throw std::logic_error{"Buffer too large!"};
    }

    void Serial_side::not_initialized()
    {
        throw std::logic_error{"Serial uninitialized!"};
    }
}
// void Serial_side::read()
// {
//     if (def_buffer == nullptr)
//     {
//         def_buffer = std::make_unique<std::vector<std::byte>>(max_buf_size);
//     }

//     if (serial == nullptr)
//     {
//         not_initialized();
//     }
//     else
//     {
//         try
//         {
//             /* Listen again */
//             serial->listen<Cont_t>(def_buffer->begin(), def_buffer->end());
//         }
//         catch (const boost::exception &e)
//         {
//             ec_callb(Exception::Serial_except{e});
//         }
//     }
// }
