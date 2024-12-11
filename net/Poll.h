#pragma once
#include "IIOMultiplex.h"

#include <poll.h>
#include <memory>

#include <vector>
#include <map>

/*
 int poll(struct pollfd *fds, nfds_t nfds, int timeout);
*/

class Poll : public IIOMultiplex {
public:
	Poll();
	~Poll();

	virtual void poll(int timeoutUs, std::vector<IEventDispatcher*>& triggerEventDispatchers) override;

	virtual void registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) override;
	virtual void registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) override;

	virtual void unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) override;
	virtual void unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) override;
	virtual void unregisterAllEvent(int fd, IEventDispatcher* eventDispatcher) override;

private:
	Poll(const Poll& rhs) = delete;
	Poll& operator=(const Poll& rhs) = delete;

	Poll(const Poll&& rhs) = delete;
	Poll& operator=(const Poll&& rhs) = delete;

private:
	struct pollfd*						m_upPollfd;
	std::map<int, IEventDispatcher*>	m_eventDispatchers;

};
