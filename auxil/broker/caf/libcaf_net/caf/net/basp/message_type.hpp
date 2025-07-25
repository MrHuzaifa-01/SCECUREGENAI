// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

#include "caf/default_enum_inspect.hpp"
#include "caf/detail/net_export.hpp"
#include "caf/fwd.hpp"
#include "caf/is_error_code_enum.hpp"

namespace caf::net::basp {

/// @addtogroup BASP

/// Describes the first header field of a BASP message and determines the
/// interpretation of the other header fields.
enum class message_type : uint8_t {
  /// Sends supported BASP version and node information to the server.
  ///
  /// ![](client_handshake.png)
  handshake = 0,

  /// Transmits an actor-to-actor messages.
  ///
  /// ![](direct_message.png)
  actor_message = 1,

  /// Tries to resolve a path on the receiving node.
  ///
  /// ![](resolve_request.png)
  resolve_request = 2,

  /// Transmits the result of a path lookup.
  ///
  /// ![](resolve_response.png)
  resolve_response = 3,

  /// Informs the receiving node that the sending node has created a proxy
  /// instance for one of its actors. Causes the receiving node to attach a
  /// functor to the actor that triggers a down_message on termination.
  ///
  /// ![](monitor_message.png)
  monitor_message = 4,

  /// Informs the receiving node that it has a proxy for an actor that has been
  /// terminated.
  ///
  /// ![](down_message.png)
  down_message = 5,

  /// Used to generate periodic traffic between two nodes in order to detect
  /// disconnects.
  ///
  /// ![](heartbeat.png)
  heartbeat = 6,
};

/// @relates message_type
CAF_NET_EXPORT std::string to_string(message_type x);

/// @relates message_type
CAF_NET_EXPORT bool from_string(string_view, message_type&);

/// @relates message_type
CAF_NET_EXPORT bool from_integer(std::underlying_type_t<message_type>,
                                 message_type&);

/// @relates message_type
template <class Inspector>
bool inspect(Inspector& f, message_type& x) {
  return default_enum_inspect(f, x);
}

/// @}

} // namespace caf::net::basp
