#include "Buffer.h"

#include <string.h>

Buffer::operator const char* () {
	return m_internalBuf.c_str();
}

void Buffer::append(const char* buf, int bufLen) {
	m_internalBuf.append(buf, bufLen);
}

void Buffer::retrieve(std::string& outBuf, size_t bufLen /*= 0*/) {
	if (bufLen == 0 || bufLen >= m_internalBuf.length()) {
		outBuf = std::move(m_internalBuf);
		return;
	}

	outBuf = m_internalBuf.substr(0, bufLen);
	m_internalBuf.erase(0, bufLen);
}

size_t Buffer::retrieve(char* buf, size_t bufLen) {
	if (bufLen == 0) {
		return 0;
	}

	if (bufLen >= m_internalBuf.size()) {
		bufLen = m_internalBuf.size();
	}

	memcpy(buf, /*Buffer::operator const char *()*/m_internalBuf.c_str(), bufLen);
	m_internalBuf.erase(0, bufLen);

	return bufLen;
}
size_t Buffer::peek(char* buf, size_t bufLen) {
	if (bufLen == 0) {
		return 0;
	}

	if (bufLen >= m_internalBuf.size()) {
		bufLen = m_internalBuf.size();
	}

	memcpy(buf, m_internalBuf.c_str(), bufLen);
	return bufLen;
}

void Buffer::erase(size_t bufLen /*= 0*/) {
	if (bufLen == 0 || bufLen >= m_internalBuf.length()) {
		m_internalBuf.clear();
		return;
	}

	m_internalBuf.erase(0, bufLen);
}

size_t Buffer::findLF() {
	return m_internalBuf.find('\n');
}

size_t Buffer::remaining() {
	return m_internalBuf.length();
}

void Buffer::clear() {
	m_internalBuf.clear();
}

bool Buffer::isEmpty() {
	return m_internalBuf.empty();
}