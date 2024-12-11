#pragma once
#include <memory>

#include "TCPConnection.h"

class TinyServer; //avoid header file circular dependency

enum class DecodePackageResult {
	DecodePackageSuccess,
	DecodePackageFailed,
	DecodePackageWantMoreData
};

class Session : public std::enable_shared_from_this<Session>{
public:
	Session(TinyServer* pServer, std::shared_ptr<TCPConnection>&& spConn);
	~Session();

	void onRead(Buffer& recvBuf);
	void onWrite();

	void onClose();

	int32_t getID() const {
		return m_id;
	}

	void sendMsg(const std::string& msg);

	void sendWelcomeMsg();

	void forceClose();

private:
	static int generateID();

private:
	DecodePackageResult decodePackage(Buffer& recvBuf);
	bool processPackage(const std::string& package);

	void processChatMsg(const std::string& package);
	bool processCmd(const std::string& package);

private:
	int32_t								m_id;
	TinyServer*							m_pServer;
	std::shared_ptr<TCPConnection>		m_spConn;
};
