#pragma once

#include "IIOMultiplex.h"

#include <sys/epoll.h>
#include <map>

class Epoll : public IIOMultiplex{
public:
	Epoll();
	~Epoll();

	virtual void poll(int timeoutUs, std::vector<IEventDispatcher*>& triggerEventDispatchers) override;

	virtual void registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) override;
	virtual void registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) override;

	virtual void unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) override;
	virtual void unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) override;
	virtual void unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher) override;

private:
	Epoll(const Epoll& rhs) = delete;
	Epoll& operator=(const Epoll&) = delete;

	Epoll(Epoll&& rhs) = delete;
	Epoll& operator=(Epoll&& rhs) = delete;

private:
	int							m_epollfd{-1};
	std::map<int, int32_t>		m_fdEventFlags;
};