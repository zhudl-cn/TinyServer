#include "Poll_.h"

Poll::Poll() {

}

Poll::~Poll() {

}

void Poll::poll(int timeoutUs, std::vector<IEventDispatcher*>& triggerEventDispatchers) {
	int timeoutMs = timeoutUs / 1000;

	int n = ::poll(&m_pollfds[0], m_pollfds.size(), timeoutMs);
	if (n < 0)
		return;

	bool enableRead = false;
	bool enableWrite = false;
	IEventDispatcher* pEventDispatcher;

	for (auto fd : m_pollfds) {
		if (fd.revents & POLLIN) {
			enableRead = true;
		} else {
			enableRead = false;
		}

		if (fd.revents & POLLOUT) {
			enableWrite = true;
		} else {
			enableWrite = false;
		}

		pEventDispatcher->enableReadWrite(enableRead, enableWrite);
		triggerEventDispatchers.push_back(pEventDispatcher);
	}
}

void Poll::registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) {
	m_pollfd.fd = fd;
	m_pollfd.events = POLLIN;

	m_pollfds.push_back(m_pollfd);
}

void Poll::registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) {
	m_pollfd.fd = fd;
	m_pollfd.events = POLLOUT;

	m_pollfds.push_back(m_pollfd);
}

void Poll::unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) {
	for (auto& pollFd : m_pollfds) {
		if (pollFd.fd == fd) {
			pollFd.events &= ~POLLIN;
			return;
		}
	}
}

void Poll::unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) {
	for (auto& pollFd : m_pollfds) {
		if (pollFd.fd == fd) {
			pollFd.events &= ~POLLIN;
			return;
		}
	}
}

void Poll::unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher) {
	for (auto& pollFd : m_pollfds) {
		if (pollFd.fd == fd) {
			pollFd.events &= ~POLLIN;
			m_pollfd.events &= ~POLLOUT;
			return;
		}
	}
}