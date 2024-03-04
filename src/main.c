/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <zephyr/timing/timing.h>

#define PORT 4242

LOG_MODULE_REGISTER(udp_server, LOG_LEVEL_DBG);

int server_init(const struct sockaddr_in6 *addr6) {
  int sock = zsock_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

  if (sock < 0) {
    LOG_ERR("socket failed: %d", -errno);
    return -1;
  }

  if (zsock_bind(sock, (struct sockaddr *)addr6, sizeof(*addr6)) < 0) {
    LOG_ERR("bind failed: %d", -errno);
    zsock_close(sock);
    return -1;
  }

  if (zsock_listen(sock, 5) < 0) {
    LOG_ERR("listen failed: %d", -errno);
    zsock_close(sock);
    return -1;
  }

  return sock;
}

int main(void) {
  const struct sockaddr_in6 addr6 = {.sin6_family = AF_INET6,
                                     .sin6_port = htons(PORT),
                                     .sin6_addr = in6addr_any};
  int server_sock, conn_sock, ret, buf = 0;

  server_sock = server_init(&addr6);
  if (server_sock < 0) {
    return server_sock;
  }

  LOG_DBG("Waiting for connection");

  conn_sock = zsock_accept(server_sock, NULL, NULL);

  if (conn_sock < 0) {
    LOG_ERR("accept failed: %d", -errno);
    zsock_close(server_sock);
    return -1;
  }

  LOG_INF("TCP server started");

  while (true) {
    ret = zsock_recv(conn_sock, &buf, sizeof(buf), 0);
    if (ret < 0) {
      LOG_ERR("recv failed: %d", -errno);
      return -1;
    }

    LOG_INF("Got %d", buf);

    zsock_send(conn_sock, &buf, sizeof(buf), 0);
    if (ret < 0) {
      LOG_ERR("send failed: %d", -errno);
      return -1;
    }
  }

  return 0;
}
