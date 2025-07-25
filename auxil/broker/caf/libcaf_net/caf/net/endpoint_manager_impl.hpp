// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include "caf/abstract_actor.hpp"
#include "caf/actor_cast.hpp"
#include "caf/actor_system.hpp"
#include "caf/detail/overload.hpp"
#include "caf/net/endpoint_manager.hpp"

namespace caf::net {

template <class Transport>
class endpoint_manager_impl : public endpoint_manager {
public:
  // -- member types -----------------------------------------------------------

  using super = endpoint_manager;

  using transport_type = Transport;

  using application_type = typename transport_type::application_type;

  using read_result = typename super::read_result;

  using write_result = typename super::write_result;

  // -- constructors, destructors, and assignment operators --------------------

  endpoint_manager_impl(const multiplexer_ptr& parent, actor_system& sys,
                        socket handle, Transport trans)
    : super(handle, parent, sys), transport_(std::move(trans)) {
    // nop
  }

  ~endpoint_manager_impl() override {
    // nop
  }

  // -- properties -------------------------------------------------------------

  transport_type& transport() {
    return transport_;
  }

  endpoint_manager_impl& manager() {
    return *this;
  }

  // -- interface functions ----------------------------------------------------

  error init() /*override*/ {
    this->register_reading();
    return transport_.init(*this);
  }

  read_result handle_read_event() override {
    return transport_.handle_read_event(*this);
  }

  write_result handle_write_event() override {
    if (!this->queue_.blocked()) {
      this->queue_.fetch_more();
      auto& q = std::get<0>(this->queue_.queue().queues());
      do {
        q.inc_deficit(q.total_task_size());
        for (auto ptr = q.next(); ptr != nullptr; ptr = q.next()) {
          auto f = detail::make_overload(
            [&](endpoint_manager_queue::event::resolve_request& x) {
              transport_.resolve(*this, x.locator, x.listener);
            },
            [&](endpoint_manager_queue::event::new_proxy& x) {
              transport_.new_proxy(*this, x.peer, x.id);
            },
            [&](endpoint_manager_queue::event::local_actor_down& x) {
              transport_.local_actor_down(*this, x.observing_peer, x.id,
                                          std::move(x.reason));
            },
            [&](endpoint_manager_queue::event::timeout& x) {
              transport_.timeout(*this, x.type, x.id);
            });
          visit(f, ptr->value);
        }
      } while (!q.empty());
    }
    if (!transport_.handle_write_event(*this)) {
      if (this->queue_.blocked())
        return write_result::stop;
      else if (!(this->queue_.empty() && this->queue_.try_block()))
        return write_result::again;
      else
        return write_result::stop;
    }
    return write_result::again;
  }

  void handle_error(sec code) override {
    transport_.handle_error(code);
  }

private:
  transport_type transport_;

  /// Stores the id for the next timeout.
  uint64_t next_timeout_id_;

  error err_;
};

} // namespace caf::net
