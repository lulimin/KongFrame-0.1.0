// windows_net.h
// Created by lulimin on 2019/2/14.

#ifndef __WINDOWS_NET_H
#define __WINDOWS_NET_H

#include <winsock2.h>
#include <windows.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

#define PLAT_INVALID_SOCKET INVALID_SOCKET

typedef SOCKET plat_socket_t;

// Initialize network communication.
inline bool plat_net_startup()
{
	WSADATA wsa;	
	
	return WSAStartup(0x202, &wsa) == 0;
}

// Shutdown network communication.
inline bool plat_net_cleanup()
{
	return WSACleanup() == 0;
}

// Create UDP communication.
inline bool plat_open_udp_sock(plat_socket_t* pSock)
{
	plat_socket_t sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (INVALID_SOCKET == sock)
	{
		return false;
	}
	
	*pSock = sock;
	return true;
}

// Create TCP communication.
inline bool plat_open_tcp_sock(plat_socket_t* pSock)
{
	plat_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == sock)
	{
		return false;
	}

	*pSock = sock;
	return true;
}

// Close connection.
inline bool plat_close_sock(plat_socket_t sock)
{
	int res = closesocket(sock);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
			
	return true;
}

// Binding port.
inline bool plat_bind_sock(plat_socket_t sock, const char* addr, int port)
{
	sockaddr_in sa;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons((u_short)port);

	int res = bind(sock, (const sockaddr*)&sa, sizeof(sa));

	if (SOCKET_ERROR == res)
	{
		return false;
	}
			
	return true;
}

// Listening port.
inline bool plat_listen_sock(plat_socket_t sock, int backlog)
{
	int res = listen(sock, backlog);

	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// Accept connection.
inline bool plat_accept_sock(plat_socket_t sock, plat_socket_t* pNewSock,
	char* pRemoteAddr, size_t remote_addr_size, int* pRemotePort)
{
	sockaddr_in	sa;
	int	len = sizeof(sockaddr);
	SOCKET new_sock = accept(sock, (sockaddr*)&sa, &len);

	if (INVALID_SOCKET == new_sock)
	{
		return false;
	}
	
	*pNewSock = new_sock;
	
	if (pRemoteAddr)
	{
		char* addr = inet_ntoa(sa.sin_addr);
		size_t addr_len = strlen(addr);
		
		if (addr_len >= remote_addr_size)
		{
			memcpy(pRemoteAddr, addr, remote_addr_size - 1);
			pRemoteAddr[remote_addr_size - 1] = 0;
		}
		else
		{
			memcpy(pRemoteAddr, addr, addr_len + 1);
		}
	}
	
	if (pRemotePort)
	{
		*pRemotePort = ntohs(sa.sin_port);
	}
	
	return true;
}

// Connect to remote.
inline bool plat_connect_sock(plat_socket_t sock, const char* addr, int port)
{
	sockaddr_in sa;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);

	int res = connect(sock, (sockaddr*)&sa, sizeof(sockaddr));

	if (SOCKET_ERROR == res)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			return false;
		}
	}

	return true;
}

// Stop send and receive.
inline bool plat_shutdown_sock(plat_socket_t sock)
{
	return shutdown(sock, SD_BOTH) != SOCKET_ERROR;
}

// Stop send.
inline bool plat_shutdown_sock_send(plat_socket_t sock)
{
	return shutdown(sock, SD_SEND) != SOCKET_ERROR;
}

// Send message.
inline int plat_send_sock(plat_socket_t sock, const char* buffer, size_t size)
{
	int res = send(sock, buffer, (int)size, 0);
	
	if (SOCKET_ERROR == res)
	{
		return -1;
	}
	
	return res;
}

// Asyncronize send message.
inline int plat_send_sock_async(plat_socket_t sock, const char* buffer,
	size_t size, bool* pWouldBlock)
{
	int res = send(sock, buffer, (int)size, 0);
	
	if (SOCKET_ERROR == res)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			*pWouldBlock = true;
		}
		else
		{
			*pWouldBlock = false;
		}
		
		return -1;
	}
	else
	{
		*pWouldBlock = false;
	}
	
	return res;
}

// Receive message.
inline bool plat_receive_sock(plat_socket_t sock, char* pBuffer, size_t size,
	size_t* pReadSize)
{
	int res = recv(sock, pBuffer, (int)size, 0);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	*pReadSize = res;
	return true;
}

// Send message specified address.
inline int plat_sendto_sock(plat_socket_t sock, const char* buffer, 
	size_t size, const char* addr, int port)
{
	sockaddr_in	sa;
	
	memset(&sa, 0, sizeof(sa));	
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);
	
	int res = sendto(sock, buffer, (int)size, 0, (sockaddr*)&sa, 
		sizeof(sockaddr));
	
	if (SOCKET_ERROR == res)
	{
		return -1;
	}
	
	return res;
}
	
// Receive message and peer address.
inline bool plat_recvfrom_sock(plat_socket_t sock, char* pBuffer, size_t size,
	char* pRemoteAddr, size_t remote_addr_size, int* pRemotePort, 
	size_t* pReadSize)
{
	sockaddr_in sa;
	int len = sizeof(sockaddr);
	int res = recvfrom(sock, pBuffer, (int)size, 0, (sockaddr*)&sa, &len);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	*pReadSize = res;

	if (pRemoteAddr)
	{
		char* addr = inet_ntoa(sa.sin_addr);
		size_t addr_len = strlen(addr);
		
		if (addr_len >= remote_addr_size)
		{
			memcpy(pRemoteAddr, addr, remote_addr_size - 1);
			pRemoteAddr[remote_addr_size - 1] = 0;
		}
		else
		{
			memcpy(pRemoteAddr, addr, addr_len + 1);
		}
	}
	
	if (pRemotePort)
	{
		*pRemotePort = ntohs(sa.sin_port);
	}
	
	return true;
}

// Monitor event.
inline bool plat_select_sock(plat_socket_t sock, int wait_ms, 
	bool* pReadFlag, bool* pWriteFlag, bool* pExceptFlag)
{
	fd_set read_fs;
	fd_set write_fs;
	fd_set except_fs;

	FD_ZERO(&read_fs);
	FD_ZERO(&write_fs);
	FD_ZERO(&except_fs);
	FD_SET(sock, &read_fs);
	FD_SET(sock, &write_fs);
	FD_SET(sock, &except_fs);
	
	timeval tv = { 0, wait_ms * 1000 };
	int res = select(2, &read_fs, &write_fs, &except_fs, &tv);

	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	*pReadFlag = false;
	*pWriteFlag = false;
	*pExceptFlag = false;

	if (res > 0)
	{
		if (FD_ISSET(sock, &except_fs))
		{
			*pExceptFlag = true;
		}
		
		if (FD_ISSET(sock, &write_fs))
		{
			*pWriteFlag = true;
		}
		
		if (FD_ISSET(sock, &read_fs))
		{
			*pReadFlag = true;
		}
	}
	
	return true;
}

// Monitor read event.
inline bool plat_select_sock_read(plat_socket_t sock, int wait_ms,
	bool* pReadFlag, bool* pExceptFlag)
{
	fd_set read_fs;
	fd_set except_fs;

	FD_ZERO(&read_fs);
	FD_ZERO(&except_fs);
	FD_SET(sock, &read_fs);
	FD_SET(sock, &except_fs);

	timeval tv = { 0, wait_ms * 1000 };
	int res = select(2, &read_fs, NULL, &except_fs, &tv);

	if (SOCKET_ERROR == res)
	{
		return false;
	}

	*pReadFlag = false;
	*pExceptFlag = false;

	if (res > 0)
	{
		if (FD_ISSET(sock, &except_fs))
		{
			*pExceptFlag = true;
		}

		if (FD_ISSET(sock, &read_fs))
		{
			*pReadFlag = true;
		}
	}

	return true;
}

// Set nonblocking mode.
inline bool plat_set_sock_nonblocking(plat_socket_t sock)
{
	u_long nb = 1;
	int res = ioctlsocket(sock, FIONBIO, &nb);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// Set blocking mode.
inline bool plat_set_sock_blocking(plat_socket_t sock)
{
	u_long nb = 0;
	int res = ioctlsocket(sock, FIONBIO, &nb);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// Set reuse address.
inline bool plat_set_sock_reuse_addr(plat_socket_t sock)
{
	int flag = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag,
		sizeof(flag));
		
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// Set broadcast.
inline bool plat_set_sock_broadcast(plat_socket_t sock)
{
	int flag = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&flag,
		sizeof(flag));
		
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// Set no delay send.
inline bool plat_set_sock_no_delay(plat_socket_t sock)
{
	int flag = 1;
	int res = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag,
		sizeof(flag));
		
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// Get local address of connection.
inline bool plat_get_sock_name(plat_socket_t sock, char* pAddr, 
	size_t addr_size, int* pPort)
{
	sockaddr_in	sa;
	int	len = sizeof(sockaddr);
	int res = getsockname(sock, (sockaddr*)&sa, &len);

	if (SOCKET_ERROR == res)
	{
		return false;
	}

	if (pAddr)
	{
		char* s = inet_ntoa(sa.sin_addr);
		size_t s_len = strlen(s);

		if (s_len >= addr_size)
		{
			memcpy(pAddr, s, addr_size - 1);
			pAddr[addr_size - 1] = 0;
		}
		else
		{
			memcpy(pAddr, s, s_len + 1);
		}
	}

	if (pPort)
	{
		*pPort = ntohs(sa.sin_port);
	}

	return true;
}

// Get peer address of connection.
inline bool plat_get_sock_peer_name(plat_socket_t sock, char* pAddr,
	size_t addr_size, int* pPort)
{
	sockaddr_in	sa;
	int	len = sizeof(sockaddr);
	int res = getpeername(sock, (sockaddr*)&sa, &len);

	if (SOCKET_ERROR == res)
	{
		return false;
	}

	if (pAddr)
	{
		char* s = inet_ntoa(sa.sin_addr);
		size_t s_len = strlen(s);

		if (s_len >= addr_size)
		{
			memcpy(pAddr, s, addr_size - 1);
			pAddr[addr_size - 1] = 0;
		}
		else
		{
			memcpy(pAddr, s, s_len + 1);
		}
	}

	if (pPort)
	{
		*pPort = ntohs(sa.sin_port);
	}

	return true;
}

// Get error information.
inline const char* plat_get_sock_error(char* pBuffer, size_t size)
{
	int err = WSAGetLastError();
	
	if (FormatMessageA(
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		0, err, 0, pBuffer, (DWORD)size, NULL) == 0)
	{
		pBuffer[0] = 0;
	}
		
	return pBuffer;
}

// Get broadcast bind address.
inline const char* plat_get_broadcast_bind_addr(const char* local_addr,
	const char* broad_addr)
{
	return local_addr;
}

#endif // __WINDOWS_NET_H
