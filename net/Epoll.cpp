#include "Epoll.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

#include <iostream>

#include "utils.h"

Epoll::Epoll() {
	m_epollfd = ::epoll_create1(EPOLL_CLOEXEC);
	if (m_epollfd < 0)
		crash();
}

Epoll::~Epoll() {
	if (m_epollfd > 0) {
		::close(m_epollfd);
	}
}

/**
 * @brief Wrapper epoll_wait, then store dispatchers into vector
 *		  TODO: use other datastructor
 * @param timeoutUs 
 * @param triggerEventDispatchers 
 */
void Epoll::poll(int timeoutUs, std::vector<IEventDispatcher*>& triggerEventDispatchers) {
	struct epoll_event events[1024];
	int timeoutMs = timeoutUs / 1000;

	bool enableRead = false;
	bool enableWrite = false;

	int n = ::epoll_wait(m_epollfd, events, 1024, timeoutMs);
	IEventDispatcher* pEventDispatcher;

	for (int i = 0; i < n; ++i) {
		if (events[i].events & EPOLLIN)
			enableRead = true;
		else
			enableRead = false;

		if (events[i].events & EPOLLOUT)
			enableWrite = true;
		else
			enableWrite = false;

		pEventDispatcher = static_cast<IEventDispatcher*>(events[i].data.ptr);
		pEventDispatcher->enableReadWrite(enableRead, enableWrite);
		triggerEventDispatchers.push_back(pEventDispatcher);
	}
}

/**
 * @brief wrapper epoll_ctl. Check the flag before do registeration 
 * @param fd 
 * @param eventDispatcher 
 * @param readEvent 
 */
void Epoll::registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) {
	int32_t eventFlag = 0;

	auto iter = m_fdEventFlags.find(fd);
	if (iter == m_fdEventFlags.end()) {	// not be registered before 
		eventFlag |= EPOLLIN;
		m_fdEventFlags[fd] = eventFlag;
	} else {
		eventFlag = iter->second;
		if (eventFlag & EPOLLIN) { // has beed registered as read event
			return;
		}

		eventFlag |= EPOLLIN;
		m_fdEventFlags[fd] = eventFlag;
	}
	
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = eventFlag;
	event.data.ptr = eventDispatcher;
	if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event) < 0) {
		crash();
	}
}

void Epoll::registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) {
	int32_t eventFlag = 0;

	auto iter = m_fdEventFlags.find(fd);
	if (iter == m_fdEventFlags.end()) {	// not be registered before 
		eventFlag |= EPOLLOUT;
		m_fdEventFlags[fd] = eventFlag;
	} else {
		eventFlag = iter->second;
		if (eventFlag & EPOLLOUT) { // has beed registered as write event
			return;
		}

		eventFlag |= EPOLLOUT;
		m_fdEventFlags[fd] = eventFlag;
	}

	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = eventFlag;
	event.data.ptr = eventDispatcher;
	if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event) < 0) {
		crash();
	}
}

void Epoll::unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) {
	int32_t eventFlag = 0;
	int operation;

	auto iter = m_fdEventFlags.find(fd);
	if (iter == m_fdEventFlags.end()) {
		return;
	} else {
		eventFlag = iter->second;
		if (!(eventFlag & EPOLLIN)) {
			return;
		}

		eventFlag &= ~EPOLLIN;
		
		if (eventFlag == 0) {	// only read event
			m_fdEventFlags.erase(iter);
			operation = EPOLL_CTL_DEL;
		} else {				// has other events, just remove read event
			m_fdEventFlags[fd] = eventFlag;
			operation = EPOLL_CTL_MOD;
		}
	}

	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = eventFlag;
	event.data.ptr = eventDispatcher;
	if (::epoll_ctl(m_epollfd, operation, fd, &event) < 0) {
		crash();
	}
}

void Epoll::unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent) {
	int32_t eventFlag = 0;
	int operation;

	auto iter = m_fdEventFlags.find(fd);
	if (iter == m_fdEventFlags.end()) {
		return;
	} else {
		eventFlag = iter->second;
		if (!(eventFlag & EPOLLOUT)) {
			return;
		}

		eventFlag &= ~EPOLLOUT;

		if (eventFlag == 0) {	// only write event
			m_fdEventFlags.erase(iter);
			operation = EPOLL_CTL_DEL;
		} else {				// has other events, just remove write event
			m_fdEventFlags[fd] = eventFlag;
			operation = EPOLL_CTL_MOD;
		}
	}

	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = eventFlag;
	event.data.ptr = eventDispatcher;
	if (::epoll_ctl(m_epollfd, operation, fd, &event) < 0) {
		crash();
	}
}

void Epoll::unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher) {
	int32_t eventFlag = 0;
	int operation;

	auto iter = m_fdEventFlags.find(fd);
	if (iter == m_fdEventFlags.end()) {
		return;
	} else {
		m_fdEventFlags.erase(iter);
	}

	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = eventFlag;
	event.data.ptr = eventDispatcher;
	if (::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &event) < 0) {
		crash();
	}
}