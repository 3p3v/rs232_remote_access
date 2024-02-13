#include <Job.hpp>

namespace Job_ctrl
{
    Id_type::~Id_type() = default;
    Id_counter::Job_id Id_counter::shared_id{0};
}