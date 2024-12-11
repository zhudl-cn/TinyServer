#pragma once

#include "IIOMultiplex.h"
#include <sys/select.h>
#include <map>

class Select :public IIOMultiplex {
public:
	Select();
	~Select() = default;

	void poll(int timeoutUs, std::vector<IEventDispatcher*>& triggerEventDispatchers) override;

	void registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) override;
	void registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) override;

	void unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) override;
	void unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) override;
	void unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher) override;

private:
	Select(const Select& rhs) = delete;
	Select& operator=(const Select& rhs) = delete;

	Select(Select&& rhs) = delete;
	Select& operator=(Select&& rhs) = delete;

private:
	int			m_maxfd{ 0 };
	fd_set		m_readset;
	fd_set		m_writeset;
	fd_set		m_excptset;

	std::vector<int> m_fds;
};
