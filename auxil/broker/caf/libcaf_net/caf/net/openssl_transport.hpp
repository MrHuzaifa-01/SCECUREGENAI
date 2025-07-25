// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include "caf/byte_buffer.hpp"
#include "caf/defaults.hpp"
#include "caf/detail/has_after_reading.hpp"
#include "caf/fwd.hpp"
#include "caf/logger.hpp"
#include "caf/net/fwd.hpp"
#include "caf/net/handshake_worker.hpp"
#include "caf/net/multiplexer.hpp"
#include "caf/net/receive_policy.hpp"
#include "caf/net/stream_oriented_layer_ptr.hpp"
#include "caf/net/stream_socket.hpp"
#include "caf/net/stream_transport.hpp"
#include "caf/sec.hpp"
#include "caf/settings.hpp"
#include "caf/span.hpp"
#include "caf/tag/io_event_oriented.hpp"
#include "caf/tag/stream_oriented.hpp"

CAF_PUSH_WARNINGS
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
CAF_POP_WARNINGS

#include <memory>
#include <string>
#include <string_view>

// -- small wrappers to help working with OpenSSL ------------------------------

namespace caf::net::openssl {

struct deleter {
  void operator()(SSL_CTX* ptr) const noexcept {
    SSL_CTX_free(ptr);
  }

  void operator()(SSL* ptr) const noexcept {
    SSL_free(ptr);
  }
};

/// A smart pointer to an `SSL_CTX` structure.
/// @note technically, SSL structures are reference counted and we could use
///       `intrusive_ptr` instead. However, we have no need for shared ownership
///       semantics here and use `unique_ptr` for simplicity.
using ctx_ptr = std::unique_ptr<SSL_CTX, deleter>;

/// A smart pointer to an `SSL` structure.
/// @note technically, SSL structures are reference counted and we could use
///       `intrusive_ptr` instead. However, we have no need for shared ownership
///       semantics here and use `unique_ptr` for simplicity.
using conn_ptr = std::unique_ptr<SSL, deleter>;

/// Convenience function for creating an OpenSSL context for given method.
inline ctx_ptr make_ctx(const SSL_METHOD* method) {
  if (auto ptr = SSL_CTX_new(method))
    return ctx_ptr{ptr};
  else
    CAF_RAISE_ERROR("SSL_CTX_new failed");
}

/// Fetches a string representation for the last OpenSSL errors.
inline std::string fetch_error_str() {
  auto cb = [](const char* cstr, size_t len, void* vptr) -> int {
    auto& str = *reinterpret_cast<std::string*>(vptr);
    if (str.empty()) {
      str.assign(cstr, len);
    } else {
      str += "; ";
      auto view = std::string_view{cstr, len};
      str.insert(str.end(), view.begin(), view.end());
    }
    return 1;
  };
  std::string result;
  ERR_print_errors_cb(cb, &result);
  return result;
}

///  Loads the certificate into the SSL context.
inline error certificate_pem_file(const ctx_ptr& ctx, const std::string& path) {
  ERR_clear_error();
  auto cstr = path.c_str();
  if (SSL_CTX_use_certificate_file(ctx.get(), cstr, SSL_FILETYPE_PEM) > 0) {
    return none;
  } else {
    return make_error(sec::invalid_argument, fetch_error_str());
  }
}

///  Loads the private key into the SSL context.
inline error private_key_pem_file(const ctx_ptr& ctx, const std::string& path) {
  ERR_clear_error();
  auto cstr = path.c_str();
  if (SSL_CTX_use_PrivateKey_file(ctx.get(), cstr, SSL_FILETYPE_PEM) > 0) {
    return none;
  } else {
    return make_error(sec::invalid_argument, fetch_error_str());
  }
}

/// Convenience function for creating a new SSL structure from given context.
inline conn_ptr make_conn(const ctx_ptr& ctx) {
  if (auto ptr = SSL_new(ctx.get()))
    return conn_ptr{ptr};
  else
    CAF_RAISE_ERROR("SSL_new failed");
}

/// Convenience function for creating a new SSL structure from given context and
/// binding the given socket to it.
/// @note the connection does *not* take ownership of the socket.
inline conn_ptr make_conn(const ctx_ptr& ctx, stream_socket fd) {
  if (auto bio_ptr = BIO_new_socket(fd.id, BIO_NOCLOSE)) {
    auto ptr = make_conn(ctx);
    SSL_set_bio(ptr.get(), bio_ptr, bio_ptr);
    return ptr;
  } else {
    CAF_RAISE_ERROR("BIO_new_socket failed");
  }
}

/// Manages an OpenSSL connection.
class policy {
public:
  // -- constructors, destructors, and assignment operators --------------------

  policy() = delete;

  policy(const policy&) = delete;

  policy& operator=(const policy&) = delete;

  policy(policy&&) = default;

  policy& operator=(policy&&) = default;

  explicit policy(conn_ptr conn) : conn_(std::move(conn)) {
    // nop
  }

  // -- factories --------------------------------------------------------------

  /// Creates a policy from an SSL context and socket.
  static policy make(const ctx_ptr& ctx, stream_socket fd) {
    return policy{make_conn(ctx, fd)};
  }

  /// Creates a policy from an SSL method and socket.
  static policy make(const SSL_METHOD* method, stream_socket fd) {
    auto ctx = make_ctx(method);
    return policy{make_conn(ctx, fd)};
  }

  // -- properties -------------------------------------------------------------

  SSL* conn() {
    return conn_.get();
  }

  // -- OpenSSL settings -------------------------------------------------------

  ///  Loads the certificate into the SSL connection object.
  error certificate_pem_file(const std::string& path) {
    ERR_clear_error();
    auto cstr = path.c_str();
    if (SSL_use_certificate_file(conn(), cstr, SSL_FILETYPE_PEM) > 0) {
      return none;
    } else {
      return make_error(sec::invalid_argument, fetch_error_str());
    }
  }

  ///  Loads the private key into the SSL connection object.
  error private_key_pem_file(const std::string& path) {
    ERR_clear_error();
    auto cstr = path.c_str();
    if (SSL_use_PrivateKey_file(conn(), cstr, SSL_FILETYPE_PEM) > 0) {
      return none;
    } else {
      return make_error(sec::invalid_argument, fetch_error_str());
    }
  }

  // -- interface functions for the stream transport ---------------------------

  /// Fetches a string representation for the last error.
  std::string fetch_error_str() {
    return openssl::fetch_error_str();
  }

  /// Reads data from the SSL connection into the buffer.
  ptrdiff_t read(stream_socket, span<byte> buf) {
    ERR_clear_error();
    return SSL_read(conn_.get(), buf.data(), static_cast<int>(buf.size()));
  }

  /// Writes data from the buffer to the SSL connection.
  ptrdiff_t write(stream_socket, span<const byte> buf) {
    ERR_clear_error();
    return SSL_write(conn_.get(), buf.data(), static_cast<int>(buf.size()));
  }

  /// Performs a TLS/SSL handshake with the server.
  ptrdiff_t connect(stream_socket) {
    ERR_clear_error();
    return SSL_connect(conn_.get());
  }

  /// Waits for the client to performs a TLS/SSL handshake.
  ptrdiff_t accept(stream_socket) {
    ERR_clear_error();
    return SSL_accept(conn_.get());
  }

  /// Returns the last SSL error.
  stream_transport_error last_error(stream_socket fd, ptrdiff_t ret) {
    switch (SSL_get_error(conn_.get(), static_cast<int>(ret))) {
      case SSL_ERROR_NONE:
      case SSL_ERROR_WANT_ACCEPT:
      case SSL_ERROR_WANT_CONNECT:
        // For all of these, OpenSSL docs say to do the operation again later.
        return stream_transport_error::temporary;
      case SSL_ERROR_SYSCALL:
        // Need to consult errno, which we just leave to the default policy.
        return default_stream_transport_policy::last_error(fd, ret);
      case SSL_ERROR_WANT_READ:
        return stream_transport_error::want_read;
      case SSL_ERROR_WANT_WRITE:
        return stream_transport_error::want_write;
      default:
        // Errors like SSL_ERROR_WANT_X509_LOOKUP are technically temporary, but
        // we do not configure any callbacks. So seeing this is a red flag.
        return stream_transport_error::permanent;
    }
  }

  /// Graceful shutdown.
  void notify_close() {
    SSL_shutdown(conn_.get());
  }

  /// Returns the number of bytes that are buffered internally and that are
  /// available for immediate read.
  size_t buffered() {
    return static_cast<size_t>(SSL_pending(conn_.get()));
  }

private:
  /// Our SSL connection data.
  openssl::conn_ptr conn_;
};

/// Asynchronously starts the TLS/SSL client handshake.
/// @param fd A connected stream socket.
/// @param mpx Pointer to the multiplexer for managing the asynchronous events.
/// @param pol The OpenSSL policy with properly configured SSL/TSL method.
/// @param on_success The callback for creating a @ref socket_manager after
///                   successfully connecting to the server.
/// @param on_error The callback for unexpected errors.
/// @pre `fd != invalid_socket`
/// @pre `mpx != nullptr`
template <class Socket, class OnSuccess, class OnError>
socket_manager_ptr async_connect(Socket fd, multiplexer* mpx, policy pol,
                                 OnSuccess on_success, OnError on_error) {
  using res_t = decltype(on_success(fd, mpx, std::move(pol)));
  using err_t = decltype(on_error(error{}));
  static_assert(std::is_convertible_v<res_t, socket_manager_ptr>,
                "on_success must return a socket_manager_ptr");
  static_assert(std::is_same_v<err_t, void>,
                "on_error may not return anything");
  using factory_t = default_handshake_worker_factory<OnSuccess, OnError>;
  using worker_t = handshake_worker<false, Socket, policy, factory_t>;
  auto factory = factory_t{std::move(on_success), std::move(on_error)};
  auto mgr = make_counted<worker_t>(fd, mpx, std::move(pol),
                                    std::move(factory));
  mpx->init(mgr);
  return mgr;
}

/// Asynchronously starts the TLS/SSL server handshake.
/// @param fd A connected stream socket.
/// @param mpx Pointer to the multiplexer for managing the asynchronous events.
/// @param pol The OpenSSL policy with properly configured SSL/TSL method.
/// @param on_success The callback for creating a @ref socket_manager after
///                   successfully connecting to the client.
/// @param on_error The callback for unexpected errors.
/// @pre `fd != invalid_socket`
/// @pre `mpx != nullptr`
template <class Socket, class OnSuccess, class OnError>
socket_manager_ptr async_accept(Socket fd, multiplexer* mpx, policy pol,
                                OnSuccess on_success, OnError on_error) {
  using res_t = decltype(on_success(fd, mpx, std::move(pol)));
  using err_t = decltype(on_error(error{}));
  static_assert(std::is_convertible_v<res_t, socket_manager_ptr>,
                "on_success must return a socket_manager_ptr");
  static_assert(std::is_same_v<err_t, void>,
                "on_error may not return anything");
  using factory_t = default_handshake_worker_factory<OnSuccess, OnError>;
  using worker_t = handshake_worker<true, Socket, policy, factory_t>;
  auto factory = factory_t{std::move(on_success), std::move(on_error)};
  auto mgr = make_counted<worker_t>(fd, mpx, std::move(pol),
                                    std::move(factory));
  mpx->init(mgr);
  return mgr;
}

} // namespace caf::net::openssl

namespace caf::net {

/// Implements a stream_transport that manages a stream socket with encrypted
/// communication over OpenSSL.
template <class UpperLayer>
class openssl_transport
  : public stream_transport_base<openssl::policy, UpperLayer> {
public:
  // -- member types -----------------------------------------------------------

  using super = stream_transport_base<openssl::policy, UpperLayer>;

  // -- constructors, destructors, and assignment operators --------------------

  template <class... Ts>
  explicit openssl_transport(openssl::conn_ptr conn, Ts&&... xs)
    : super(openssl::policy{std::move(conn)}, std::forward<Ts>(xs)...) {
    // nop
  }

  template <class... Ts>
  openssl_transport(openssl::policy policy, Ts&&... xs)
    : super(std::move(policy), std::forward<Ts>(xs)...) {
    // nop
  }
};

} // namespace caf::net
