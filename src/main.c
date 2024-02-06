/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>

#define PORT 4242

LOG_MODULE_REGISTER(udp_server, LOG_LEVEL_DBG);

int server_init(const struct sockaddr_in6 *addr6) {
  int sock = zsock_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

  if (sock < 0) {
    LOG_ERR("socket failed: %d", -errno);
    return -1;
  }

  if (zsock_bind(sock, (struct sockaddr *)addr6, sizeof(*addr6)) < 0) {
    LOG_ERR("bind failed: %d", -errno);
    zsock_close(sock);
    return -1;
  }

  return sock;
}

int main(void) {
  const struct sockaddr_in6 addr6 = {.sin6_family = AF_INET6,
                                     .sin6_port = htons(PORT),
                                     .sin6_addr = in6addr_any};
  uint8_t buf[100] = {0};
  int n, sock, count = 0;

  sock = server_init(&addr6);
  if (sock < 0) {
    return sock;
  }

  LOG_INF("UDP server started");

  while (true) {
    n = zsock_recvfrom(sock, buf, sizeof(buf), 0, NULL, 0);
    if (n < 0) {
      LOG_ERR("recvfrom failed: %d", -errno);
      break;
    }
    count++;
    LOG_DBG("Received %d bytes, Msg count: %d", n, count);
  }

  return 0;
}
