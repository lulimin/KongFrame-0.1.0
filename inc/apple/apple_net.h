// apple_net.h
// Created by lulimin on 2019/2/14.

#ifndef __APPLE_NET_H
#define __APPLE_NET_H

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define PLAT_INVALID_SOCKET -1

typedef int plat_socket_t;

// Initialize network communication.
inline bool plat_net_startup()
{
	return true;
}

// Shutdown network communication.
inline bool plat_net_cleanup()
{
	return true;
}

// Create UDP communication.
inline bool plat_open_udp_sock(plat_socket_t* pSock)
{
	plat_socket_t sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (-1 == sock)
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

	if (-1 == sock)
	{
		return false;
	}

	*pSock = sock;
	return true;
}

// Close connection.
inline bool plat_close_sock(plat_socket_t sock)
{
	int res = close(sock);
	
	if (-1 == res)
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

	if (-1 == res)
	{
		return false;
	}
			
	return true;
}

// Listen port.
inline bool plat_listen_sock(plat_socket_t sock, int backlog)
{
	int res = listen(sock, backlog);

	if (-1 == res)
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
	socklen_t len = sizeof(sockaddr);
	int new_sock = accept(sock, (sockaddr*)&sa, &len);

	if (-1 == new_sock)
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
		*pRemotePort = sa.sin_port;
	}
	
	return true;
}

// Connect to peer.
inline bool plat_connect_sock(plat_socket_t sock, const char* addr, int port)
{
	sockaddr_in sa;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);

	int res = connect(sock, (sockaddr*)&sa, sizeof(sockaddr));

	if (-1 == res)
	{
		if (errno != EINPROGRESS)
		{
			return false;
		}
	}

	return true;
}

// Stop send and receive.
inline bool plat_shutdown_sock(plat_socket_t sock)
{
	return shutdown(sock, SHUT_RDWR) == 0;
}

// Stop send.
inline bool plat_shutdown_sock_send(plat_socket_t sock)
{
	return shutdown(sock, SHUT_WR) == 0;
}

// Send message.
inline int plat_send_sock(plat_socket_t sock, const char* buffer, size_t size)
{
	ssize_t res = send(sock, buffer, size, 0);
	
	if (-1 == res)
	{
		return -1;
	}
	
	return (int)res;
}

// Asyncronize send message.
inline int plat_send_sock_async(plat_socket_t sock, const char* buffer,
	size_t size, bool* pWouldBlock)
{
	ssize_t res = send(sock, buffer, size, 0);
	
	if (-1 == res)
	{
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
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
	
	return (int)res;
}

// Receive message.
inline bool plat_receive_sock(plat_socket_t sock, char* pBuffer, size_t size,
	size_t* pReadSize)
{
	ssize_t res = recv(sock, pBuffer, size, 0);
	
	if (-1 == res)
	{
		return false;
	}
	
	*pReadSize = res;
	return true;
}

// Send message to remote address.
inline int plat_sendto_sock(plat_socket_t sock, const char* buffer, 
	size_t size, const char* addr, int port)
{
	sockaddr_in	sa;
	
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);
	
	ssize_t res = sendto(sock, buffer, (int)size, 0, (sockaddr*)&sa, 
		sizeof(sockaddr));
	
	if (-1 == res)
	{
		return -1;
	}
	
	return (int)res;
}
	
// Receive messsage and remote address.
inline bool plat_recvfrom_sock(plat_socket_t sock, char* pBuffer, size_t size,
	char* pRemoteAddr, size_t remote_addr_size, int* pRemotePort,
	size_t* pReadSize)
{
	sockaddr_in sa;
	socklen_t len = sizeof(sockaddr);
	ssize_t res = recvfrom(sock, pBuffer, (int)size, 0, (sockaddr*)&sa, &len);
	
	if (-1 == res)
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
	// Select under linux have problem while fd exceed 1024, must use poll.
	struct pollfd pfd;
	
	pfd.fd = sock;
	pfd.events = POLLIN | POLLOUT;
	pfd.revents = 0;
	
	int res = poll(&pfd, 1, wait_ms);
	
	if (-1 == res)
	{
		return false;
	}
	
	*pReadFlag = false;
	*pWriteFlag = false;
	*pExceptFlag = false;

	if (res > 0)
	{
		if (pfd.revents & POLLIN)
		{
			*pReadFlag = true;
		}
		
		if (pfd.revents & POLLOUT)
		{
			*pWriteFlag = true;
		}
		
		if (pfd.revents & (POLLHUP | POLLERR))
		{
			*pExceptFlag = true;
		}
	}
	
	return true;
}

// Monitor read event.
inline bool plat_select_sock_read(plat_socket_t sock, int wait_ms,
	bool* pReadFlag, bool* pExceptFlag)
{
	// Select under linux have problem while fd exceed 1024, must use poll.
	struct pollfd pfd;

	pfd.fd = sock;
	pfd.events = POLLIN;
	pfd.revents = 0;

	int res = poll(&pfd, 1, wait_ms);

	if (-1 == res)
	{
		return false;
	}

	*pReadFlag = false;
	*pExceptFlag = false;

	if (res > 0)
	{
		if (pfd.revents & POLLIN)
		{
			*pReadFlag = true;
		}

		if (pfd.revents & (POLLHUP | POLLERR))
		{
			*pExceptFlag = true;
		}
	}

	return true;
}

// Set nonblocking mode.
inline bool plat_set_sock_nonblocking(plat_socket_t sock)
{
	int flags = fcntl(sock, F_GETFL);
	
	if (-1 == flags)
	{
		return false;
	}
	
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		return false;
	}
	
	return true;
}

// Set blocking mode.
inline bool plat_set_sock_blocking(plat_socket_t sock)
{
	int flags = fcntl(sock, F_GETFL);
	
	if (-1 == flags)
	{
		return false;
	}
	
	if (fcntl(sock, F_SETFL, flags & ~O_NONBLOCK) == -1)
	{
		return false;
	}
	
	return true;
}

// Set can reuse address.
inline bool plat_set_sock_reuse_addr(plat_socket_t sock)
{
	int flag = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag,
		sizeof(flag));
		
	if (-1 == res)
	{
		return false;
	}
	
	return true;
}

// Set can broadcast.
inline bool plat_set_sock_broadcast(plat_socket_t sock)
{
	int flag = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&flag,
		sizeof(flag));
		
	if (-1 == res)
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
		
	if (-1 == res)
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
	socklen_t len = sizeof(sockaddr);
	int res = getsockname(sock, (sockaddr*)&sa, &len);

	if (-1 == res)
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
	socklen_t len = sizeof(sockaddr);
	int res = getpeername(sock, (sockaddr*)&sa, &len);

	if (-1 == res)
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
	if (strerror_r(errno, pBuffer, size) != 0)
	{
		pBuffer[0] = 0;
	}

	return pBuffer;
}

// Get bind address in broadcast mode.
inline const char* plat_get_broadcast_bind_addr(const char* local_addr,
	const char* broad_addr)
{
	return broad_addr;
}

#endif // __APPLE_NET_H
