// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include <vector>

#include "caf/actor_control_block.hpp"
#include "caf/actor_proxy.hpp"
#include "caf/binary_deserializer.hpp"
#include "caf/config.hpp"
#include "caf/detail/scope_guard.hpp"
#include "caf/detail/sync_request_bouncer.hpp"
#include "caf/execution_unit.hpp"
#include "caf/logger.hpp"
#include "caf/message.hpp"
#include "caf/message_id.hpp"
#include "caf/net/basp/header.hpp"
#include "caf/node_id.hpp"

namespace caf::net::basp {

template <class Subtype>
class remote_message_handler {
public:
  void handle_remote_message(execution_unit* ctx) {
    // Local variables.
    auto& dref = static_cast<Subtype&>(*this);
    auto& payload = dref.payload_;
    auto& hdr = dref.hdr_;
    auto& registry = dref.system_->registry();
    auto& proxies = *dref.proxies_;
    CAF_LOG_TRACE(CAF_ARG(hdr) << CAF_ARG2("payload.size", payload.size()));
    // Deserialize payload.
    actor_id src_id = 0;
    node_id src_node;
    actor_id dst_id = 0;
    std::vector<strong_actor_ptr> fwd_stack;
    message content;
    binary_deserializer source{ctx, payload};
    if (!(source.apply(src_node) && source.apply(src_id) && source.apply(dst_id)
          && source.apply(fwd_stack) && source.apply(content))) {
      CAF_LOG_ERROR(
        "failed to deserialize payload:" << CAF_ARG(source.get_error()));
      return;
    }
    // Sanity checks.
    if (dst_id == 0)
      return;
    // Try to fetch the receiver.
    auto dst_hdl = registry.get(dst_id);
    if (dst_hdl == nullptr) {
      CAF_LOG_DEBUG("no actor found for given ID, drop message");
      return;
    }
    // Try to fetch the sender.
    strong_actor_ptr src_hdl;
    if (src_node != none && src_id != 0)
      src_hdl = proxies.get_or_put(src_node, src_id);
    // Ship the message.
    auto ptr = make_mailbox_element(std::move(src_hdl),
                                    make_message_id(hdr.operation_data),
                                    std::move(fwd_stack), std::move(content));
    dref.queue_->push(ctx, dref.msg_id_, std::move(dst_hdl), std::move(ptr));
  }
};

} // namespace caf::net::basp
