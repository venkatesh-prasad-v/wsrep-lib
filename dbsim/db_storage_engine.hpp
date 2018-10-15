/*
 * Copyright (C) 2018 Codership Oy <info@codership.com>
 *
 * This file is part of wsrep-lib.
 *
 * Wsrep-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Wsrep-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wsrep-lib.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef WSREP_DB_STORAGE_ENGINE_HPP
#define WSREP_DB_STORAGE_ENGINE_HPP

#include "db_params.hpp"

#include "wsrep/mutex.hpp"
#include "wsrep/client_state.hpp"

#include <atomic>
#include <unordered_set>

namespace db
{
    class client;
    class storage_engine
    {
    public:
        storage_engine(const params& params)
            : mutex_()
            , transactions_()
            , alg_freq_(params.alg_freq)
            , bf_aborts_()
        { }

        class transaction
        {
        public:
            transaction(storage_engine& se)
                : se_(se)
                , cc_()
            { }
            ~transaction()
            {
                rollback();
            }
            bool active() const { return cc_ != nullptr; }
            void start(client* cc);
            void apply(const wsrep::transaction&);
            void commit();
            void rollback();
            db::client* client() { return cc_; }
            transaction(const transaction&) = delete;
            transaction& operator=(const transaction&) = delete;
        private:
            db::storage_engine& se_;
            db::client* cc_;
        };
        void bf_abort_some(const wsrep::transaction& tc);
        long long bf_aborts() const { return bf_aborts_; }
    private:
        wsrep::default_mutex mutex_;
        std::unordered_set<db::client*> transactions_;
        size_t alg_freq_;
        std::atomic<long long> bf_aborts_;
    };
}

#endif // WSREP_DB_STORAGE_ENGINE_HPP
