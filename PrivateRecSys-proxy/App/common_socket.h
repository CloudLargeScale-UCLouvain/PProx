#ifndef SERVER_COMMON_SOCKET_H
#define SERVER_COMMON_SOCKET_H

#include <string>

void print_dbg(int csock, int sock, std::string out, std::string done, const char *str);

int do_socket();

void do_setsockopt(int sock);

void do_bind(int sock, uint16_t port);

void do_listen(int sock);

int do_accept(int sock);

int do_connect(const char *addr, const char *port);

void epoll_ctl_add(int epfd, int fd, uint32_t events);

int do_recv(int sock, char *buffer);

int do_send(int sock, char *buffer, int size);

#endif //SERVER_COMMON_SOCKET_H
