//
// Copyright (C) 2018 Codership Oy <info@codership.com>
//

#ifndef TRREP_CONDITION_VARIABLE_HPP
#define TRREP_CONDITION_VARIABLE_HPP

#include "lock.hpp"

#include <cstdlib>

namespace trrep
{
    class condition_variable
    {
    public:
        condition_variable() { }
        virtual ~condition_variable() { }
        virtual void notify_one() = 0;
        virtual void notify_all() = 0;
        virtual void wait(trrep::unique_lock<trrep::mutex>& lock) = 0;
    private:
        condition_variable(const condition_variable&);
        condition_variable& operator=(const condition_variable&);
    };

    // Default pthreads based condition variable implementation
    class default_condition_variable : public condition_variable
    {
    public:
        default_condition_variable()
            : cond_()
        {
            if (pthread_cond_init(&cond_, 0))
            {
                throw trrep::runtime_error("Failed to initialized condvar");
            }
        }

        ~default_condition_variable()
        {
            if (pthread_cond_destroy(&cond_))
            {
                ::abort();
            }
        }
        void notify_one()
        {
            (void)pthread_cond_signal(&cond_);
        }

        void notify_all()
        {
            (void)pthread_cond_broadcast(&cond_);
        }

        void wait(trrep::unique_lock<trrep::mutex>& lock)
        {
            if (pthread_cond_wait(
                    &cond_,
                    reinterpret_cast<pthread_mutex_t*>(lock.mutex().native())))
            {
                throw trrep::runtime_error("Cond wait failed");
            }
        }

    private:
        pthread_cond_t cond_;
    };

}

#endif // TRREP_CONDITION_VARIABLE_HPP
