// frame_services.h
// Created by lulimin on 2019/2/18.

#ifndef __FRAME_SERVICES_H
#define __FRAME_SERVICES_H

#include "../inc/common.h"

class IService;
class IServiceCreator;
struct frame_services_t;

// Create service manager.
frame_services_t* frame_services_create();
// Delete service manager.
void frame_services_delete(frame_services_t* pServices);

// Destroy all services.
void frame_services_release_all(frame_services_t* pServices);
// Add service creator.
bool frame_services_add_creator(frame_services_t* pServices, 
	IServiceCreator* pCreator);
// Search service.
bool frame_services_find(frame_services_t* pServices, const char* name);
// Create service or increase service reference count.
IService* frame_services_get_reference(frame_services_t* pServices, 
	const char* name);
// Release service or decrease service reference count.
void frame_services_release_reference(frame_services_t* pServices, 
	IService* pSvc);
// Dump all service informations.
bool frame_services_dump(frame_services_t* pServices, const char* file_name);

#endif // __FRAME_SERVICES_H
