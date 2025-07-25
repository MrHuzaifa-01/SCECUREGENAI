// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "caf/actor.hpp"
#include "caf/actor_clock.hpp"
#include "caf/detail/net_export.hpp"
#include "caf/fwd.hpp"
#include "caf/intrusive/drr_queue.hpp"
#include "caf/intrusive/fifo_inbox.hpp"
#include "caf/intrusive/singly_linked.hpp"
#include "caf/mailbox_element.hpp"
#include "caf/net/endpoint_manager_queue.hpp"
#include "caf/net/socket_manager.hpp"
#include "caf/variant.hpp"

namespace caf::net {

/// Manages a communication endpoint.
class CAF_NET_EXPORT endpoint_manager : public socket_manager {
public:
  // -- member types -----------------------------------------------------------

  using super = socket_manager;

  // -- constructors, destructors, and assignment operators --------------------

  endpoint_manager(socket handle, const multiplexer_ptr& parent,
                   actor_system& sys);

  ~endpoint_manager() override;

  // -- properties -------------------------------------------------------------

  actor_system& system() noexcept {
    return sys_;
  }

  const actor_system_config& config() const noexcept;

  // -- queue access -----------------------------------------------------------

  bool at_end_of_message_queue();

  endpoint_manager_queue::message_ptr next_message();

  // -- event management -------------------------------------------------------

  /// Resolves a path to a remote actor.
  void resolve(uri locator, actor listener);

  /// Enqueues a message to the endpoint.
  void enqueue(mailbox_element_ptr msg, strong_actor_ptr receiver);

  /// Enqueues an event to the endpoint.
  template <class... Ts>
  void enqueue_event(Ts&&... xs) {
    enqueue(new endpoint_manager_queue::event(std::forward<Ts>(xs)...));
  }

  // -- pure virtual member functions ------------------------------------------

  /// Initializes the manager before adding it to the multiplexer's event loop.
  // virtual error init() = 0;

protected:
  bool enqueue(endpoint_manager_queue::element* ptr);

  /// Points to the hosting actor system.
  actor_system& sys_;

  /// Stores control events and outbound messages.
  endpoint_manager_queue::type queue_;

  /// Stores a proxy for interacting with the actor clock.
  actor timeout_proxy_;
};

using endpoint_manager_ptr = intrusive_ptr<endpoint_manager>;

} // namespace caf::net
