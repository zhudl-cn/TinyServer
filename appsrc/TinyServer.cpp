#include "TinyServer.h"

#include <memory>
#include <iostream>

bool TinyServer::init(int32_t threadNum, const std::string& ip, uint16_t port)
{
	m_tcpServer.setConnectedCallback(std::bind(&TinyServer::onConnected, this, std::placeholders::_1));

	if (!m_tcpServer.init(threadNum, ip, port)) {
		return false;
	}

	//TODO: Timer

	m_tcpServer.start();

	return true;
}

void TinyServer::uninit(){
	m_tcpServer.uninit();
}

void TinyServer::sendAll(const std::string& msg, bool includeSelf, int32_t id){
	for (const auto& iter : m_sessions) {
		if (!includeSelf) {
			if (iter.second->getID() == id) {
				continue;
			}
		}

		iter.second->sendMsg(msg);
	}
}

void TinyServer::onDisconnected(int32_t id){
	auto iter = m_sessions.find(id);
	if (iter != m_sessions.end()) {
		auto pendingToDeleteSession = iter->second;
		m_pendingToDeleteSessions.push_back(std::move(pendingToDeleteSession));

		m_sessions.erase(iter);
		std::cout << "client[" << id << "] disconnected" << std::endl;
	}
}

void TinyServer::onConnected(std::shared_ptr<TCPConnection>& spConn){
	m_pendingToDeleteSessions.clear();

	auto spSession = std::make_shared<Session>(this, std::move(spConn));
	spSession->sendWelcomeMsg();

	m_sessions.emplace(spSession->getID(), std::move(spSession));
}

