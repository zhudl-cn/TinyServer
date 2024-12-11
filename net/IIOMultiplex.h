#pragma once
#include <vector>

#include "IEventDispatcher.h"

class IIOMultiplex {
public:
	//TODO: could switch IEventDispatcher to unique pointer

	/**
	 * @brief wait event on a fd
	 * @param timeoutUs - microseconds (1000,000us = 1s)
	 * @param triggerEventDispatchers
	 */
	virtual void poll(int timeoutUs, std::vector<IEventDispatcher*>& triggerEventDispatchers) = 0;

	virtual void registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) = 0;
	virtual void registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) = 0;

	virtual void unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) = 0;
	virtual void unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent) = 0;
	virtual void unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher) = 0;
};