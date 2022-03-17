// link_creator.h
// Created by lulimin on 2020/2/18.

#ifndef __LINK_CREATOR_H
#define __LINK_CREATOR_H

class IServiceCreator;
class IObjectCreator;

extern void link_flat_creators(IServiceCreator**, IObjectCreator**);
extern void link_flatex_creators(IServiceCreator**, IObjectCreator**);
extern void link_helper_creators(IServiceCreator**, IObjectCreator**);
extern void link_render_creators(IServiceCreator**, IObjectCreator**);

// Link creators of service and object.
inline void link_creators(IServiceCreator** svc, IObjectCreator** obj)
{
	link_flat_creators(svc, obj);
	link_flatex_creators(svc, obj);
	link_helper_creators(svc, obj);
	link_render_creators(svc, obj);
}

#endif // __LINK_CREATOR_H
