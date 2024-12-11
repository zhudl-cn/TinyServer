#pragma once
#include <functional>
#include <string>

#include "EventLoop.h"
#include "IEventDispatcher.h"

#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 8888

using AcceptCallback = std::function<void(int clientfd)>;

class Acceptor final : public IEventDispatcher {
public:
	Acceptor(EventLoop* pEventLoop);
	~Acceptor();

	Acceptor(const Acceptor& rhs);
	Acceptor& operator=(const Acceptor& rhs);

	Acceptor(Acceptor&& rhs);
	Acceptor& operator=(Acceptor&& rhs);

	void setAcceptCallback(AcceptCallback&& callback) {
		m_acceptCallback = callback;
	}

	virtual void onRead() override;
	bool startListen(const std::string& ip = DEFAULT_IP_ADDRESS, uint16_t port = DEFAULT_PORT);

private:
	virtual void onWrite() override {};
	virtual void onClose() override {};
	virtual void enableReadWrite(bool read, bool write) override {};

private:
	int						m_listenfd;
	EventLoop*				m_pEventLoop;
	AcceptCallback			m_acceptCallback;
};
