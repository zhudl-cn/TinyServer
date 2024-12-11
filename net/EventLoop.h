#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "IIOMultiplex.h"
#include "IEventDispatcher.h"

using CustomTask = std::function<void()>;

enum class IOMultiplexType {
	IOMultiplexTypeSelect,
	IOMultiplexTypePoll,
	IOMultiplexTypeEpoll
};

class EventLoop final{
public:
	EventLoop() = default;
	~EventLoop() = default;

	void init(IOMultiplexType type = IOMultiplexType::IOMultiplexTypeEpoll);
	void run();

	void addTask(const CustomTask& task);

	void setThreadID(const std::thread::id& threadID);
	const std::thread::id& getThreadID() const;

	bool isCallableInOwnerThread() const;

	void registerReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent);
	void registerWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent);
	void unregisterReadEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent);
	void unregisterWriteEvent(int fd, IEventDispatcher* eventDispatcher, bool readEvent);
	void unregisterAllEvents(int fd, IEventDispatcher* eventDispatcher);

	void doOtherTasks();

private:
	EventLoop(const EventLoop& rhs) = delete;
	EventLoop& operator=(const EventLoop& rhs) = delete;

	EventLoop(EventLoop&& rhs) = delete;
	EventLoop& operator=(EventLoop&& rhs) = delete;

private:
	bool							m_running{false};
	std::shared_ptr<IIOMultiplex>	m_spIOMultiplex;

	std::thread::id					m_threadID;

	std::mutex						m_mutexTasks;
	std::vector<CustomTask>			m_customTask;
};