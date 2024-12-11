#include "TCPConnection.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>

#include <functional>
#include <iostream>

#include "EventLoop.h"
#include "Buffer.h"

#define MAX_BUFFER_SIZE 1024

TCPConnection::TCPConnection(int clientfd, const std::shared_ptr<EventLoop>& spEventLoop)
	: m_fd(clientfd), m_spEventLoop(spEventLoop){}

TCPConnection::~TCPConnection() {
	::close(m_fd);
}

bool TCPConnection::startRead() {
	m_spEventLoop->registerReadEvent(m_fd, this, true);

	return true;
}

bool TCPConnection::send(const char* buf, int buflen) {
	return send(std::string(buf, buflen));
}

bool TCPConnection::send(const std::string& buf) {
	if (isCallableInOwnerThread()) {
		return sendInternal(buf.c_str(), buf.length());
	} else {
		CustomTask t = std::bind(static_cast<bool(TCPConnection::*)(const std::string&)>(&TCPConnection::send), this, buf);
		m_spEventLoop->addTask(t);
	}
	return true;
}

void TCPConnection::onRead() {
	if (!m_enableRead)
		return;

	char buf[MAX_BUFFER_SIZE];
	int n = ::recv(m_fd, buf, sizeof(buf), 0);
	if (n == 0) {
		onClose();
	} else if (n < 0) {
		if (errno == EINTR)
			return;

		onClose();
		return;
	}

	m_recvBuf.append(buf, n);

	std::cout << "ClientFD[" << m_fd << "] says: " << m_recvBuf;

	m_readCallback(m_recvBuf);
}

void TCPConnection::onWrite() {
	if (!m_enableWrite)
		return;

	while (true) {
		int n = ::send(m_fd, m_sendBuf, m_sendBuf.remaining(), 0);
		if (n == 0) {
			//client closes connection
			return;
		} else if (n < 0) {
			if (errno == EINTR) {
				continue;
			} else if (errno == EWOULDBLOCK || errno == EAGAIN) { //tcp window is full or small
				//return to user layer,
				m_writeCallback();
				//stop write into data
				unregisterWriteEvent();

				return;
			}
			return;
		}

		m_sendBuf.erase(n);
		if (m_sendBuf.isEmpty()) {
			return;
		}
	}
}

void TCPConnection::onClose() {
	unregisterAllEvents();
	m_closeCallback(shared_from_this());
}

void TCPConnection::enableReadWrite(bool read, bool write) {
	m_enableRead = read;
	m_enableWrite = write;
}

bool TCPConnection::sendInternal(const char* buf, int bufLen) {
	m_sendBuf.append(buf, bufLen);

	while (true) {
		int n = ::send(m_fd, m_sendBuf, m_sendBuf.remaining(), 0);
		if (n == 0) {
			//client closes connection
			onClose();
			return false;
		} else if (n < 0) {
			if (errno == EINTR) {
				continue;
			} else if (errno == EWOULDBLOCK || errno == EAGAIN) { //tcp window is full or small 
				//return to user layer, 
				m_writeCallback();
				//continue to write into data, wait for next write
				registerWriteEvent();

				return true;
			}

			onClose();
			return false;
		}

		m_sendBuf.erase(n);
		if (m_sendBuf.isEmpty()) {
			return true;
		}
	}

	return false;
}

void TCPConnection::registerWriteEvent() {
	if (m_registerWriteEvent)
		return;

	m_spEventLoop->registerWriteEvent(m_fd, this, true);
	m_registerWriteEvent = true;
}

void TCPConnection::unregisterWriteEvent() {
	if (!m_registerWriteEvent)
		return;

	m_spEventLoop->unregisterWriteEvent(m_fd, this, false);
	m_registerWriteEvent = false;
}

void TCPConnection::unregisterAllEvents() {
	m_spEventLoop->unregisterAllEvents(m_fd, this);
	m_registerWriteEvent = false;
}

bool TCPConnection::isCallableInOwnerThread() {
	return std::this_thread::get_id() == m_spEventLoop->getThreadID();
}