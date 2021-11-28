#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "user_types.h"
#include <thread>
#include "Enclave_u.h"

bool DEBUG = (atoi(std::getenv("DEBUG")) != 0);

void print_dbg(int csock, int sock, std::string out, std::string done, const char *str) {
	if (DEBUG) {
		char hostname[1024];
		char buffer[26];
		time_t timer = time(NULL);
		gethostname(hostname, 1024);
		std::chrono::time_point <std::chrono::system_clock> now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
		strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", localtime(&timer));
		if (str != NULL) {
			printf("%li | %s | %s |\n%s\n", nanoseconds.count(), hostname, buffer, str);
		} else {
			printf("%li | %s | %s | [%i] %s [%i] %s\n", nanoseconds.count(), hostname, buffer, csock, out.c_str(), sock, done.c_str());
		}
		fflush(stdout);
	}
}

void ocall_print_string(const char *str) {
	print_dbg(0, 0, "", "", str);
}

int do_socket() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		if (DEBUG) {
			perror("socket()");
		}
		exit(errno);
	}
	return sock;
}

void do_setsockopt(int sock) {
	int enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		if (DEBUG) {
			perror("setsockopt()");
		}
		close(sock);
		exit(errno);
	}
}

void do_bind(int sock, uint16_t port) {
	struct sockaddr_in sin;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *) &sin, sizeof(sin)) != 0) {
		if (DEBUG) {
			perror("bind()");
		}
		exit(errno);
	}
}

void do_listen(int sock) {
	if (listen(sock, SOMAXCONN) < 0) {
		if (DEBUG) {
			perror("listen()");
		}
		exit(errno);
	}
}

int do_accept(int sock) {
	struct sockaddr_in csin;
	socklen_t sinsize = sizeof(csin);
	int csock = accept(sock, (struct sockaddr *) &csin, &sinsize);
	if (csock < 1) {
		if (DEBUG) {
			perror("accept()");
		}
		exit(errno);
	}
	return csock;
}

int do_connect(const char *addr, const char *port) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd = -1;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(addr, port, &hints, &result) != 0) {
		if (DEBUG) {
			perror("getaddrinfo()");
		}
		exit(errno);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		close(sfd);
	}
	freeaddrinfo(result);
	if (sfd < 0) {
		if (DEBUG) {
			printf("do_connect(%s:%s) failed\n", addr, port);
			fflush(stdout);
			exit(errno);
		}
	}
	return sfd;
}

void epoll_ctl_add(int epfd, int fd, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		if (DEBUG) {
			perror("epoll_ctl()");
		}
		exit(errno);
	}
}

int handle_send(int sock, char *buffer, int size) {
	int i = (int) send(sock, buffer, size, 0);
	if (i < 0) {
		if (DEBUG) {
			perror("send()");
			fflush(stdout);
		}
		//exit(errno);
	}
	if (i == 0) {
		if (DEBUG) {
			printf("[%i] Client disconnected - send()\n", sock);
			fflush(stdout);
		}
		//exit(errno);
	}
	return i;
}

int handle_recv(int sock, char *buffer, int size) {
	int i = (int) recv(sock, buffer, size, 0);
	if (i < 0) {
		if (DEBUG) {
			perror("recv()");
			fflush(stdout);
		}
		//exit(errno);
	}
	if (i == 0) {
		if (DEBUG) {
			printf("[%i] Client disconnected - recv()\n", sock);
			fflush(stdout);
		}
		//exit(errno);
	}
	return i;
}

int do_send(int sock, char *buffer, int size) {
	int total = 0;
	int sent = 0;
	while (total < size && sent >= 0) {
		sent = handle_send(sock, buffer + total, size - total);
		total += sent;
	}
	return total;
}

void ocall_send(int sock, char *msg, size_t size) {
	do_send(sock, msg, (int) size);
}

int do_recv(int sock, char *buffer) {
	int total = 0;
	int recvd = 0;
	std::string buffstr;
	size_t CL = std::string::npos, rn = std::string::npos;
	do {
		recvd = handle_recv(sock, buffer + total, MSG_LENGTH - total);
		total += recvd;
		if (recvd > 0 && total > 16) {
			buffstr = std::string(buffer, 0, total);
			CL = buffstr.find("Content-Length: ");
			rn = buffstr.find("\r\n\r\n");
		} else {
			return 0;
		}
	} while ((CL == std::string::npos && buffstr.substr(total - 4, total) != "\r\n\r\n") || (CL != std::string::npos && rn == std::string::npos) || (CL != std::string::npos && rn != std::string::npos && total < (std::stoi(buffstr.substr(CL + 16, buffstr.find("\r\n", CL))) + (int) rn + 4)));
	buffer[total] = 0;
	return total;
}
