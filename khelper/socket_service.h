// socket_service.h
// Created by lulimin on 2019/5/17.

#ifndef __SOCKET_SERVICE_H
#define __SOCKET_SERVICE_H

#include "../inc/frame_def.h"

// Socket system.

class SocketService : public IService
{
public:
	SocketService();
	virtual ~SocketService();

	virtual bool Startup();
	virtual bool Shutdown();
};

#endif // __SOCK_SYSTEM_H
