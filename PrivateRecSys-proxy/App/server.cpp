#include "concurrentqueue.h" // https://github.com/cameron314/concurrentqueue
#include "common_socket.h"
#include "Enclave_u.h"
#include "user_types.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <random>

extern sgx_enclave_id_t global_eid;

extern std::string hst;
extern std::string prt;

const int NB_THREADS = atoi(std::getenv("NB_THREADS"));
const int EPOLL_SIZE = atoi(std::getenv("EPOLL_SIZE"));
const int BUFFER_SHUFFLING = atoi(std::getenv("BUFFER_SHUFFLING"));

class Socks {
public:
	Socks(int c, int s, int r) : c_(c), s_(s), r_(r) {}
	Socks() : c_(-1), s_(-1), r_(-1) {}
	int getC() const { return c_; }
	int getS() const { return s_; }
	int getR() const { return r_; }
	void setC(int val) { c_ = val; }
	void setS(int val) { s_ = val; }
	void setR(int val) { r_ = val; }
private:
	int c_;
	int s_;
	int r_;
};

void message_handler(int csock, int sock, int ifreq) {
	std::string out = (ifreq == 0) ? ">>>" : "<<<";
	print_dbg(csock, sock, out, "Receiving and transferring...", NULL);
	char buff_final[MSG_LENGTH] = {0};
	char buff[MSG_LENGTH] = {0};
	size_t size2 = 0;
	size_t size = (ifreq == 0) ? do_recv(csock, buff) : do_recv(sock, buff);
	print_dbg(csock, sock, out, "Received.", NULL);
	print_dbg(0, 0, "", "", buff);
	if (size > 0) {
		ecall_proxy(global_eid, csock, buff, size, buff_final, &size2, ifreq);
	}
	print_dbg(csock, sock, out, "Sending...", NULL);
	print_dbg(0, 0, "", "", buff_final);
	if (ifreq == 0) {
		do_send(sock, buff_final, (int) size2);
	} else {
		do_send(csock, buff_final, (int) size2);
		close(sock);
		close(csock);
	}
	print_dbg(csock, sock, out, "Transferred!", NULL);
}

void poll_handler(int ssock) {
	int nfds = -1, epfd = -1, waiting = 0, i = 0;
	int clientsfds[65536] = {0};
	Socks buffer[BUFFER_SHUFFLING];
	moodycamel::ConcurrentQueue<Socks> cq(EPOLL_SIZE + BUFFER_SHUFFLING);
	struct epoll_event events[EPOLL_SIZE];
	epfd = epoll_create1(0);
	epoll_ctl_add(epfd, ssock, EPOLLIN);
	for (int k = 0; k < NB_THREADS; k++) {
		std::thread([&]() {
			Socks s;
			while (true) {
				while (!cq.try_dequeue(s)) { continue; }
				message_handler(s.getC(), s.getS(), s.getR());
			}
		}).detach();
	}
	while (true) { // LOOP
		nfds = epoll_wait(epfd, events, EPOLL_SIZE, 1000);
		i = 0;
		do { // iterate over epoll results
			if (nfds > 0) { // incoming request(s) or answer(s)
				if (events[i].data.fd == ssock) { // [C]-->[S] request
					int sckS = do_connect(hst.c_str(), prt.c_str());
					int sckC = do_accept(ssock);
					clientsfds[sckS] = sckC;
					fcntl(sckS, F_SETFD, fcntl(sckS, F_GETFD, 0) | O_NONBLOCK);
					epoll_ctl_add(epfd, sckS, EPOLLIN | EPOLLET);
					buffer[waiting++] = Socks(sckC, sckS, 0);
				} else { // [C]<--[S] answer
					int sckS = events[i].data.fd;
					int sckC = clientsfds[sckS];
					clientsfds[sckS] = -1;
					epoll_ctl(epfd, EPOLL_CTL_DEL, sckS, NULL);
					buffer[waiting++] = Socks(sckC, sckS, 1);
				}
			}
			if (nfds == 0 || waiting == BUFFER_SHUFFLING) { // timeout or buffer full
				shuffle(&(buffer[0]), &(buffer[waiting]), std::default_random_engine(0));
				cq.enqueue_bulk(buffer, waiting);
				waiting = 0;
			}
			i++;
		} while (i < nfds); // epoll results
	} // LOOP
}

void server_loop(uint16_t port) {
	int ssock = do_socket();
	do_setsockopt(ssock);
	do_bind(ssock, port);
	do_listen(ssock);
	poll_handler(ssock);
}

