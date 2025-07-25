// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include "caf/byte_span.hpp"
#include "caf/detail/net_export.hpp"
#include "caf/net/http/header_fields_map.hpp"
#include "caf/net/http/status.hpp"
#include "caf/string_view.hpp"

#include <utility>

namespace caf::net::http::v1 {

/// Tries splitting the given byte span into an HTTP header (`first`) and a
/// remainder (`second`). Returns an empty @ref string_view as `first` for
/// incomplete HTTP headers.
CAF_NET_EXPORT std::pair<string_view, byte_span> split_header(byte_span bytes);

/// Writes an HTTP header to the buffer.
CAF_NET_EXPORT void write_header(status code, const header_fields_map& fields,
                                 byte_buffer& buf);

/// Writes a complete HTTP response to the buffer. Automatically sets
/// Content-Type and Content-Length header fields.
CAF_NET_EXPORT void write_response(status code, string_view content_type,
                                   string_view content, byte_buffer& buf);

/// Writes a complete HTTP response to the buffer. Automatically sets
/// Content-Type and Content-Length header fields followed by the user-defined
/// @p fields.
CAF_NET_EXPORT void
write_response(status code, string_view content_type, string_view content,
               const header_fields_map& fields, byte_buffer& buf);

} // namespace caf::net::http::v1
