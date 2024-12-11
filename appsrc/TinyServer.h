#pragma once
#include <string>
#include <memory>

#include <map>

#include "TCPConnection.h"
#include "TCPServer.h"
#include "Session.h"

class TinyServer final {
public:
	TinyServer() = default;
	~TinyServer() = default;

	bool init(int32_t threadNum, const std::string& ip = "", uint16_t port = 8888);
	void uninit();

	void sendAll(const std::string& msg, bool includeSelf, int32_t id);

	void onDisconnected(int32_t id);

private:
	void onConnected(std::shared_ptr<TCPConnection>& spConn);

private:
	TCPServer												m_tcpServer;
	std::map<int32_t, std::shared_ptr<Session>>				m_sessions;
	std::vector<std::shared_ptr<Session>>					m_pendingToDeleteSessions;
};
