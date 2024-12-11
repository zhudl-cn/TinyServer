#include "EventLoop.h"

#include <iostream>

#include "Select.h"
#include "Poll_.h"
#include "Epoll.h"

void EventLoop::init(IOMultiplexType type/*= IOMultiplexType::IOMultiplexTypeEpoll*/) {
	if (type == IOMultiplexType::IOMultiplexTypeSelect) {
		m_spIOMultiplex = std::make_shared<Select>();
	}else if (type == IOMultiplexType::IOMultiplexTypePoll) {
		m_spIOMultiplex = std::make_shared<Poll>();
	}else{
		m_spIOMultiplex = std::make_shared<Epoll>();
	}

	m_running = true;
}

void EventLoop::run(){
	std::vector<IEventDispatcher*> eventDispatchers;
	while (m_running) {
		//1. check & process timer event

		//2. call select/poll/epoll to check write&read events from fds
		eventDispatchers.clear();
		m_spIOMultiplex->poll(500000, eventDispatchers);

		//3. handle write&read events
		for (size_t i = 0; i < eventDispatchers.size(); ++i) {
			eventDispatchers[i]->onRead();
			eventDispatchers[i]->onWrite();
		}

		//4. use wakeup fd to process DIY events
		doOtherTasks();
	}
}

void EventLoop::addTask(const CustomTask& task) {
	std::lock_guard<std::mutex> scopedLock(m_mutexTasks);
	m_customTask.push_back(task);
}

void EventLoop::setThreadID(const std::thread::id& threadID) {
	m_threadID = threadID;
}

const std::thread::id& EventLoop::getThreadID() const {
	return m_threadID;
}

bool EventLoop::isCallableInOwnerThread() const {
	return std::this_thread::get_id() == m_threadID;
}

void EventLoop::registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent){
	m_spIOMultiplex->registerReadEvent(fd, eventDispatcher, readEvent);
}

void EventLoop::registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent){
	m_spIOMultiplex->registerWriteEvent(fd, eventDispatcher, writeEvent);
}

void EventLoop::unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent){
	m_spIOMultiplex->unregisterReadEvent(fd, eventDispatcher, readEvent);
}

void EventLoop::unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool writeEvent){
	m_spIOMultiplex->unregisterWriteEvent(fd, eventDispatcher, writeEvent);
}

void EventLoop::unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher){
	m_spIOMultiplex->unregisterAllEvents(fd, eventDispatcher);
}

void EventLoop::doOtherTasks() {
	std::vector<CustomTask> tasks;

	{
		std::lock_guard<std::mutex> scopedLock(m_mutexTasks);
		tasks.swap(m_customTask);
	}

	for (auto& task : tasks) {
		task();
	}
}