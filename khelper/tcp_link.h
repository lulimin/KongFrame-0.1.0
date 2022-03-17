// tcp_link.h
// Created by lulimin on 2021/1/8.

#ifndef __TCP_LINK_H
#define __TCP_LINK_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"
#include "../inc/net_socket.h"

// TCP syncronize network link.

class TcpLink : public IObject 
{
private:
	enum
	{
		STATE_IDLE,
		STATE_CONNECTED,
		STATE_FAILED,
	};

public:
	TcpLink();
	virtual ~TcpLink();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();
	virtual void Work(double frame_time);

	// Send buffer length.
	void SetSendLength(int value);
	int GetSendLength() const;

	// Receive buffer length.
	void SetReceiveLength(int value);
	int GetReceiveLength() const;

	// Test connected.
	bool GetConnected() const;
	// Connect to peer.
	bool Connect(const char* address, int port);
	// Disconnect link.
	bool Disconnect();
	// Send message.
	bool Send(const char* msg);

private:
	// Send message data.
	bool SendData(const char* pdata, size_t len);
	// Receive message data.
	bool ReceiveData(char* pdata, size_t len);

private:
	plat_socket_t m_hSocket;
	int m_nState;
	FrameString m_sAddress;
	int m_nPort;
	char* m_pSendBuffer;
	size_t m_nSendLength;
	char* m_pReceiveBuffer;
	size_t m_nReceiveLength;
	size_t m_nReceiveCount;
};

#endif // __TCP_LINK_H
