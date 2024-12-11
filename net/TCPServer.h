#pragma once

#include <string>

#include "ThreadPool.h"
#include "Acceptor.h"
#include "TCPConnection.h"

using onConnectedCallback = std::function<void(std::shared_ptr<TCPConnection>& spConn)>;
using onDisconnectedCallback = std::function<void(const std::shared_ptr<TCPConnection>& spConn)>;


class TCPServer {
public:
	TCPServer() = default;
	~TCPServer() = default;

	bool init(int32_t threadNum, const std::string& ip, uint16_t port);
	void uninit();

	void start();

	void setConnectedCallback(onConnectedCallback&& callback) {
		m_connectedCallback = std::move(callback);
	}

	void setDisconnectedCallback(onDisconnectedCallback&& callback) {
		m_disconnectedCallback = std::move(callback);
	}

private:
	void onAccept(int clientfd);
	void onDisconnected(const std::shared_ptr<TCPConnection>& spConn);

private:
	ThreadPool					m_threadPool;
	std::string					m_ip;
	uint16_t					m_port;

	EventLoop					m_baseEventLoop;

	Acceptor* m_pAcceptor;

private:
	onConnectedCallback			m_connectedCallback;
	onDisconnectedCallback		m_disconnectedCallback;
};