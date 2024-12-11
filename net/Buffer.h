#pragma once

#include <string>

class Buffer {
public:
	Buffer() = default;
	~Buffer() = default;

	Buffer(const Buffer& rhs);
	Buffer operator=(const Buffer& rhs);
	Buffer(Buffer&& rhs);
	Buffer operator=(Buffer&& rhs);

public:
	operator const char* ();

	void append(const char* buf, int bufLen);
	void retrieve(std::string& outBuf, size_t bufLen = 0);
	size_t retrieve(char* buf, size_t bufLen);
	size_t peek(char* buf, size_t bufLen);

	void erase(size_t bufLen = 0);

	size_t findLF();
	size_t remaining();
	void clear();
	bool isEmpty();

private:
	std::string			m_internalBuf;
};
