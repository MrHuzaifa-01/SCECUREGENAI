// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#include "caf/net/backend/tcp.hpp"

#include <mutex>
#include <string>

#include "caf/net/actor_proxy_impl.hpp"
#include "caf/net/basp/application.hpp"
#include "caf/net/basp/application_factory.hpp"
#include "caf/net/basp/ec.hpp"
#include "caf/net/defaults.hpp"
#include "caf/net/doorman.hpp"
#include "caf/net/ip.hpp"
#include "caf/net/make_endpoint_manager.hpp"
#include "caf/net/middleman.hpp"
#include "caf/net/socket_guard.hpp"
#include "caf/net/stream_transport.hpp"
#include "caf/net/tcp_accept_socket.hpp"
#include "caf/net/tcp_stream_socket.hpp"
#include "caf/send.hpp"

namespace caf::net::backend {

tcp::tcp(middleman& mm)
  : middleman_backend("tcp"), mm_(mm), proxies_(mm.system(), *this) {
  // nop
}

tcp::~tcp() {
  // nop
}

error tcp::init() {
  uint16_t conf_port = get_or<uint16_t>(mm_.system().config(),
                                        "caf.middleman.tcp-port",
                                        defaults::middleman::tcp_port);
  ip_endpoint ep;
  auto local_address = std::string("[::]:") + std::to_string(conf_port);
  if (auto err = detail::parse(local_address, ep))
    return err;
  auto acceptor = make_tcp_accept_socket(ep, true);
  if (!acceptor)
    return acceptor.error();
  auto acc_guard = make_socket_guard(*acceptor);
  if (auto err = nonblocking(acc_guard.socket(), true))
    return err;
  auto port = local_port(*acceptor);
  if (!port)
    return port.error();
  listening_port_ = *port;
  CAF_LOG_INFO("doorman spawned on " << CAF_ARG(*port));
  auto doorman_uri = make_uri("tcp://doorman");
  if (!doorman_uri)
    return doorman_uri.error();
  auto& mpx = mm_.mpx();
  auto mgr = make_endpoint_manager(
    mpx, mm_.system(),
    doorman{acc_guard.release(), basp::application_factory{proxies_}});
  if (auto err = mgr->init()) {
    CAF_LOG_ERROR("mgr->init() failed: " << err);
    return err;
  }
  return none;
}

void tcp::stop() {
  for (const auto& p : peers_)
    proxies_.erase(p.first);
  peers_.clear();
}

expected<endpoint_manager_ptr> tcp::get_or_connect(const uri& locator) {
  if (auto auth = locator.authority_only()) {
    auto id = make_node_id(*auth);
    if (auto ptr = peer(id))
      return ptr;
    auto host = locator.authority().host;
    if (auto hostname = get_if<std::string>(&host)) {
      for (const auto& addr : ip::resolve(*hostname)) {
        ip_endpoint ep{addr, locator.authority().port};
        auto sock = make_connected_tcp_stream_socket(ep);
        if (!sock)
          continue;
        else
          return emplace(id, *sock);
      }
    }
  }
  return sec::cannot_connect_to_node;
}

endpoint_manager_ptr tcp::peer(const node_id& id) {
  return get_peer(id);
}

void tcp::resolve(const uri& locator, const actor& listener) {
  if (auto p = get_or_connect(locator))
    (*p)->resolve(locator, listener);
  else
    anon_send(listener, p.error());
}

strong_actor_ptr tcp::make_proxy(node_id nid, actor_id aid) {
  using impl_type = actor_proxy_impl;
  using hdl_type = strong_actor_ptr;
  actor_config cfg;
  return make_actor<impl_type, hdl_type>(aid, nid, &mm_.system(), cfg,
                                         peer(nid));
}

void tcp::set_last_hop(node_id*) {
  // nop
}

uint16_t tcp::port() const noexcept {
  return listening_port_;
}

endpoint_manager_ptr tcp::get_peer(const node_id& id) {
  const std::lock_guard<std::mutex> lock(lock_);
  auto i = peers_.find(id);
  if (i != peers_.end())
    return i->second;
  return nullptr;
}

} // namespace caf::net::backend
