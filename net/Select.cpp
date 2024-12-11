#include "Select.h"

Select::Select()
{
	FD_ZERO(&m_readset);
	FD_ZERO(&m_writeset);
}

void Select::poll(int timeoutUs, std::vector<IEventDispatcher*>& triggerEventDispatchers)
{
	struct timeval timeout = { timeoutUs / 1000000, timeoutUs - timeoutUs / 1000000 * 1000000 };

	int n = select(m_maxfd + 1, &m_readset, &m_writeset, &m_excptset, &timeout);
	if (n < 0)
		return;

	IEventDispatcher* pEventDispathcer;
	bool enableRead = false;
	bool enableWrite = false;
	for (auto fd : m_fds) {
		if (FD_ISSET(fd, &m_readset)) {
			enableRead = true;
		} else {
			enableRead = false;
		}

		if (FD_ISSET(fd, &m_writeset)) {
			enableWrite = true;
		} else {
			enableWrite = false;
		}

		pEventDispathcer->enableReadWrite(enableRead, enableWrite);
		triggerEventDispatchers.push_back(pEventDispathcer);
	}
}

void Select::registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent){
	m_maxfd = m_maxfd < fd ? fd : m_maxfd;

	FD_SET(fd, &m_readset);
}

void Select::registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) {
	m_maxfd = m_maxfd < fd ? fd : m_maxfd;

	FD_SET(fd, &m_readset);
}

void Select::unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) {
	FD_CLR(fd, &m_readset);
}

void Select::unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) {
	FD_CLR(fd, &m_writeset);
}
void Select::unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher) {
	FD_CLR(fd, &m_readset);
	FD_CLR(fd, &m_writeset);
}