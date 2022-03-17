// socket_service.h
// Created by lulimin on 2019/5/17.

#include "socket_service.h"
#include "../inc/net_socket.h"

DEF_SERVICE(SocketService);

// SocketService

SocketService::SocketService()
{
}

SocketService::~SocketService()
{
}

bool SocketService::Startup()
{
	return plat_net_startup();
}

bool SocketService::Shutdown()
{
	return plat_net_cleanup();
}
