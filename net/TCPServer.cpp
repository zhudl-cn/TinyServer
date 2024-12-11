#include "TCPServer.h"

#include <functional>

#include <iostream>

#include "TCPConnection.h"
#include "utils.h"

bool TCPServer::init(int32_t threadNum, const std::string& ip, uint16_t port) {
	m_threadPool.start(threadNum);

	m_ip = ip;
	m_port = port;

	m_baseEventLoop.init();

	m_pAcceptor = new Acceptor(&m_baseEventLoop);
	if (!m_pAcceptor->startListen(ip, port)) {
		return false;
	}

	m_pAcceptor->setAcceptCallback(std::bind(&TCPServer::onAccept, this, std::placeholders::_1));

	return true;
}

void TCPServer::uninit() {
	m_threadPool.stop();
}

void TCPServer::start() {
	std::cout << "base EventLoop threadID " << std::this_thread::get_id() << std::endl;
	m_baseEventLoop.setThreadID(std::this_thread::get_id());
	m_baseEventLoop.run();
}

void TCPServer::onAccept(int clientfd) {
	std::shared_ptr<EventLoop> spEventLoop = m_threadPool.getNextEventLoop();
	auto spTCPConnection = std::make_shared<TCPConnection>(clientfd, spEventLoop);

	spTCPConnection->startRead();

	m_connectedCallback(spTCPConnection);
}

void TCPServer::onDisconnected(const std::shared_ptr<TCPConnection>& spConn) {
	m_disconnectedCallback(spConn);
}