#pragma once

class IEventDispatcher {
public:
	virtual void onRead() = 0;
	virtual void onWrite() = 0;
	virtual void onClose() = 0;
	virtual void enableReadWrite(bool read, bool write) = 0;
};