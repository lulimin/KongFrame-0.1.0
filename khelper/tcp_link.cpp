// tcp_link.cpp
// Created by lulimin on 2021/1/8.

//#include "../inc/common.h"
//#include "../inc/net_socket.h"
#include "tcp_link.h"

/// \file tcp_link.cpp
/// \brief TCP syncronize network link.

/// object: TcpLink
/// \brief TCP syncronize network link object.
DEF_OBJECT(TcpLink, IObject);

/// readonly: bool Connected
/// \brief test Connected to peer.
DEF_READ(bool, TcpLink, Connected);

/// property: int SendLength
/// \brief Send buffer length.
DEF_PROP(int, TcpLink, SendLength);
/// property: int ReceiveLength
/// \brief Receive buffer length.
DEF_PROP(int, TcpLink, ReceiveLength);

/// function: bool Connect(string address, int port)
/// \brief Connect to peer.
/// \param address IP address.
/// \param port IP port.
DEF_FUNC_2(bool, TcpLink, Connect, const char*, int);
/// function: bool Disconnect()
/// \brief Disconnect link.
DEF_FUNC_0(bool, TcpLink, Disconnect);
/// function: bool Send(string msg)
/// \brief Send message.
/// \param msg Message data.
DEF_FUNC_1(bool, TcpLink, Send, const char*);

// TcpLink

TcpLink::TcpLink()
{
	m_hSocket = PLAT_INVALID_SOCKET;
	m_nState = 0;
	m_nPort = 0;
	m_pSendBuffer = NULL;
	m_nSendLength = 0;
	m_pReceiveBuffer = NULL;
	m_nReceiveLength = 0;
	m_nReceiveCount = 0;
}

TcpLink::~TcpLink()
{
	if (m_pSendBuffer)
	{
		K_FREE(m_pSendBuffer);
	}

	if (m_pReceiveBuffer)
	{
		K_FREE(m_pReceiveBuffer);
	}
}

bool TcpLink::Startup(const IArgList& args)
{
	int send_len = args.GetInt32(0);
	int receive_len = args.GetInt32(1);

	if ((send_len < 256) || (send_len > 0x10000000))
	{
		send_len = 1024;
	}

	if ((receive_len < 256) || (receive_len > 0x10000000))
	{
		receive_len = 1024;
	}

	this->SetSendLength(send_len);
	this->SetReceiveLength(receive_len);
	frame_add_work(this);
	return true;
}

bool TcpLink::Shutdown()
{
	this->Disconnect();
	frame_remove_work(this);
	return true;
}

void TcpLink::Work(double frame_time)
{
	if (m_nState != STATE_CONNECTED)
	{
		return;
	}

	bool read_flag;
	bool except_flag;

	if (!plat_select_sock_read(m_hSocket, 0, &read_flag, &except_flag))
	{
		K_LOG("(TcpLink::Work)Select error.");
		this->Disconnect();
		frame_process_event(this, 0, "close", 
			FrameArgList() << "select", NULL);
		return;
	}

	if (except_flag)
	{
		K_LOG("(TcpLink::Work)Except error.");
		this->Disconnect();
		frame_process_event(this, 0, "close", 
			FrameArgList() << "except", NULL);
		return;
	}

	if (read_flag)
	{
		char buf[256];
		size_t read_size;

		if (!plat_receive_sock(m_hSocket, buf, sizeof(buf), &read_size))
		{
			K_LOG("(TcpLink::Work)Receive error.");
			this->Disconnect();
			frame_process_event(this, 0, "close",
				FrameArgList() << "receive", NULL);
			return;
		}

		if (0 == read_size)
		{
			this->Disconnect();
			frame_process_event(this, 0, "close", 
				FrameArgList() << "normal", NULL);
			return;
		}

		for (size_t i = 0; i < read_size; ++i)
		{
			char ch = buf[i];

			if (ch == '\n')
			{
				if (m_nReceiveCount > 0)
				{
					m_pReceiveBuffer[m_nReceiveCount] = 0;
					this->ReceiveData(m_pReceiveBuffer, m_nReceiveCount);
				}

				m_nReceiveCount = 0;
			} 
			else
			{
				if (m_nReceiveCount < (m_nReceiveLength - 1))
				{
					m_pReceiveBuffer[m_nReceiveCount++] = ch;
				}
			}
		}
	}
}

void TcpLink::SetSendLength(int value)
{
	if (value < 256)
	{
		return;
	}

	if (value == m_nSendLength)
	{
		return;
	}

	char* pNewBuffer = (char*)K_ALLOC(value);

	if (m_pSendBuffer)
	{
		K_FREE(m_pSendBuffer);
	}

	m_nSendLength = value;
	m_pSendBuffer = pNewBuffer;
}

int TcpLink::GetSendLength() const
{
	return (int)m_nSendLength;
}

void TcpLink::SetReceiveLength(int value)
{
	if (value < 256)
	{
		return;
	}

	if (value == m_nReceiveLength)
	{
		return;
	}

	if (m_nReceiveCount > value)
	{
		K_LOG("(TcpLink::SetReceiveLength)Link in receiving.");
		return;
	}

	char* pNewBuffer = (char*)K_ALLOC(value);

	if (m_pReceiveBuffer)
	{
		if (m_nReceiveCount > 0)
		{
			memcpy(pNewBuffer, m_pReceiveBuffer, m_nReceiveCount);
		}
		
		K_FREE(m_pReceiveBuffer);
	}

	m_nReceiveLength = value;
	m_pReceiveBuffer = pNewBuffer;
}

int TcpLink::GetReceiveLength() const
{
	return (int)m_nReceiveLength;
}

bool TcpLink::GetConnected() const
{
	return m_nState == STATE_CONNECTED;
}

bool TcpLink::Connect(const char* address, int port)
{
	Assert(address != NULL);

	if (m_nState != 0)
	{
		this->Disconnect();
	}

	plat_socket_t sock;

	if (!plat_open_tcp_sock(&sock))
	{
		K_LOG("(TcpLink::Connect)No resource.");
		return false;
	}

	// Syncronize connect.
	if (!plat_connect_sock(sock, address, port))
	{
		K_LOG("(TcpLink::Connect)Connect error.");
		plat_close_sock(sock);
		frame_process_event(this, 0, "connect_fail", FrameArgList(), NULL);
		return false;
	}

	m_hSocket = sock;
	m_sAddress = address;
	m_nPort = port;
	m_nState = STATE_CONNECTED;
	frame_process_event(this, 0, "connected", FrameArgList(), NULL);
	return true;
}

bool TcpLink::Disconnect()
{
	if (m_hSocket != PLAT_INVALID_SOCKET)
	{
		plat_close_sock(m_hSocket);
		m_hSocket = PLAT_INVALID_SOCKET;
	}

	m_nState = 0;
	return true;
}

bool TcpLink::ReceiveData(char* pdata, size_t len)
{
	Assert(pdata != NULL);

	frame_process_event(this, 0, "receive", FrameArgList() << pdata, NULL);
	return true;
}	

bool TcpLink::SendData(const char* pdata, size_t len)
{
	Assert(pdata != NULL);
	Assert(len > 0);

	if (m_nState != STATE_CONNECTED)
	{
		K_LOG("(TcpLink::SendData)Not connected.");
		return false;
	}

	if (len >= (m_nSendLength - 1))
	{
		K_LOG("(TcpLink::SendData)Message too big.");
		return false;
	}

	memcpy(m_pSendBuffer, pdata, len);
	m_pSendBuffer[len] = '\n';
	
	int res = plat_send_sock(m_hSocket, m_pSendBuffer, len + 1);

	if (res < 0)
	{
		K_LOG("(TcpLink::SendData)Send failed.");
		return false;
	}

	return true;
}

bool TcpLink::Send(const char* msg)
{
	Assert(msg != NULL);

	return this->SendData(msg, strlen(msg));
}
