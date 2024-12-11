#include "Acceptor.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#include <iostream>

Acceptor::Acceptor(EventLoop* pEventLoop): m_pEventLoop(pEventLoop){}

Acceptor::~Acceptor() {
	if (m_listenfd != -1)
		::close(m_listenfd);
}

void Acceptor::onRead() {
	while (true) {
		//4
		struct sockaddr clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		int clientfd = ::accept4(m_listenfd, (struct sockaddr*)&clientAddr, &clientAddrLen, SOCK_NONBLOCK);
		if (clientfd > 0) {
			m_acceptCallback(clientfd);
		} else if (clientfd == -1) {
			if (errno == EWOULDBLOCK) {
				return;
			} else {
				return;
			}
		}
	}
}

bool Acceptor::startListen(
	const std::string& ip /*= DEFAULT_IP_ADDRESS*/,
	uint16_t port /*= DEFAULT_PORT*/) {
	//1. create a listen socket
	m_listenfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (m_listenfd == -1)
		return false;

	int optval = 1;

	int res = ::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
	if (res == -1) {
		return false;
	}

	res = ::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
	if (res == -1) {
		return false;
	}

	//2. init server address
	struct sockaddr_in bindaddr;
	bindaddr.sin_family = AF_INET;
	if (ip == "") {
		bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		bindaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	}

	bindaddr.sin_port = htons(port);
	if (::bind(m_listenfd, (struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1) {
		return false;
	}

	//3. start listen
	if (::listen(m_listenfd, SOMAXCONN) == -1) {
		return false;
	}

	m_pEventLoop->registerReadEvent(m_listenfd, this, true);

	return true;
}