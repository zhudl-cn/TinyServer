#include "Session.h"

#include <string.h>
#include <functional>
#include <atomic>
#include <sstream>
#include <iostream>

#include "Buffer.h"
#include "TinyServer.h"

#define MAX_MSG_LENGTH 64

Session::Session(TinyServer* pServer, std::shared_ptr<TCPConnection>&& spConn)
	:m_pServer(pServer), m_spConn(spConn){
	m_id = Session::generateID();

	m_spConn->setReadCallback(std::bind(&Session::onRead, this, std::placeholders::_1));
	m_spConn->setWriteCallback(std::bind(&Session::onWrite, this));
	m_spConn->setCloseCallback(std::bind(&Session::onClose, this));
}

Session::~Session(){}

void Session::onRead(Buffer& recvBuf) {
	while (true) {
		DecodePackageResult result = decodePackage(recvBuf);
		if (result == DecodePackageResult::DecodePackageWantMoreData)
			return;

		if (result == DecodePackageResult::DecodePackageFailed) {
			forceClose();
			return;
		}
	}
}

void Session::onWrite() {

}

void Session::onClose() {
	m_pServer->onDisconnected(m_id);
}

void Session::sendMsg(const std::string& msg) {
	m_spConn->send(msg);
}

void Session::sendWelcomeMsg() {
	const std::string& welcomeMsg = "Welcome to use TinyServer!\n";
	m_spConn->send(welcomeMsg);
}

void Session::forceClose() {
	m_spConn->onClose();
}

int Session::generateID() {
	static std::atomic<int32_t> m_baseID{ 0 };

	int32_t resultID = ++m_baseID;
	return resultID;
}

DecodePackageResult Session::decodePackage(Buffer& recvBuf) {
	size_t positionLF = recvBuf.findLF();
	if (positionLF == std::string::npos) {
		if (recvBuf.remaining() > MAX_MSG_LENGTH)
			return DecodePackageResult::DecodePackageFailed;

		return DecodePackageResult::DecodePackageWantMoreData;
	}

	std::string currentPackage;
	recvBuf.retrieve(currentPackage, positionLF + 1);

	processPackage(currentPackage);

	return DecodePackageResult::DecodePackageSuccess;
}

bool Session::processPackage(const std::string& package) {
	if (package.empty())
		return true;

	if (package[0] != '!') {
		processChatMsg(package);
	} else {
		return processCmd(package);
	}
	return false;
}

void Session::processChatMsg(const std::string& package) {
	std::ostringstream msgWithPrefix;
	msgWithPrefix << "client[";
	msgWithPrefix << m_id;
	msgWithPrefix << "] says: ";
	msgWithPrefix << package;
	m_pServer->sendAll(msgWithPrefix.str(), false, m_id);
}

bool Session::processCmd(const std::string& package) {
	//TODO
	return false;
}