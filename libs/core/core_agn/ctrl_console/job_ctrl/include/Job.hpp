#pragma once

namespace Job_ctrl
{
    class Id_type
    {
    public:
        using Job_id = unsigned int;

        Id_type() = default;
        Id_type(const Id_type &) = default;
        Id_type &operator=(const Id_type &) = default;
        Id_type(Id_type &&) = default;
        Id_type &operator=(Id_type &&) = default;
        virtual ~Id_type() = 0;
    };

    class Job : public Id_type
    {
    public:
        virtual Job_id get_id() const = 0;
    };

    class Id_counter : public Id_type
    {
    public:
        static Job_id shared_id;
    };

    /// @brief Template for
    /// @tparam Job_D
    template <typename Job_D>
    class Job_identity : public Job, private Id_counter
    {
        static Job_id get_id_();

    public:
        virtual Job_id get_id() const override final;

        static Job_id s_get_id();
    };

    template <typename Job_D>
    inline Job::Job_id Job_identity<Job_D>::get_id_()
    {
        static int class_id = shared_id++;
        
        return class_id;
    }

    template <typename Job_D>
    inline Job::Job_id Job_identity<Job_D>::get_id() const
    {
        return get_id_();
    }

    template <typename Job_D>
    inline Job::Job_id Job_identity<Job_D>::s_get_id()
    {
        return get_id_();
    }
}