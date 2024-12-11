#pragma once
#include <memory>
#include <functional>

#include <string>

#include "Buffer.h"
#include "IEventDispatcher.h"
#include "EventLoop.h"

class TCPConnection;

using ReadCallback = std::function<void(Buffer&)>;
using WriteCallback = std::function<void()>;
using CloseCallback = std::function<void(const std::shared_ptr<TCPConnection>&)>;

class TCPConnection : public IEventDispatcher, public std::enable_shared_from_this<TCPConnection> {
public:
	TCPConnection(int clientfd, const std::shared_ptr<EventLoop>& spEventLoop);
	virtual ~TCPConnection();

	void setReadCallback(ReadCallback&& readCallback) {
		m_readCallback = std::move(readCallback);
	}

	void setWriteCallback(WriteCallback&& writeCallback) {
		m_writeCallback = std::move(writeCallback);
	}

	void setCloseCallback(CloseCallback&& closeCallback) {
		m_closeCallback = std::move(closeCallback);
	}

	bool startRead();

	bool send(const char* buf, int buflen);
	bool send(const std::string& buf);

	virtual void onRead() override;
	virtual void onWrite() override;
	virtual void onClose() override;
	virtual void enableReadWrite(bool read, bool write) override;

private:
	bool sendInternal(const char* buf, int bufLen);

	void registerWriteEvent();
	void unregisterWriteEvent();
	void unregisterAllEvents();
	bool isCallableInOwnerThread();

private:
	int									m_fd;
	bool								m_enableRead;
	bool								m_enableWrite;
	std::shared_ptr<EventLoop>			m_spEventLoop;

	Buffer								m_recvBuf;
	Buffer								m_sendBuf;

	bool								m_registerWriteEvent{ false };
	bool								m_registerReadEvent{ false };

	ReadCallback						m_readCallback;
	WriteCallback						m_writeCallback;
	CloseCallback						m_closeCallback;
};
