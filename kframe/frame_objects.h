// frame_objects.h
// Created by lulimin on 2019/2/18.

#ifndef __FRAME_OBJECTS_H
#define __FRAME_OBJECTS_H

#include "../inc/common.h"

class IObject;
class IObjectCreator;
class FrameObjectDesc;
struct frame_objects_t;

// Create object manager.
frame_objects_t* frame_objects_create();
// Delete object manager.
void frame_objects_delete(frame_objects_t* pObjects);

// Add object creator.
bool frame_objects_add_creator(frame_objects_t* pObjects, 
	IObjectCreator* pCreator);
// Set all parent object classes.
void frame_objects_set_all_parent(frame_objects_t* pObjects);
// Find object description by name.
FrameObjectDesc* frame_objects_get_desc(frame_objects_t* pObjects,
	const char* name);
// Add to execute list.
bool frame_objects_add_work(frame_objects_t* pObjects, IObject* pObj);
// Remove from execute list.
bool frame_objects_remove_work(frame_objects_t* pObjects, IObject* pObj);
// Execute object work per frame.
void frame_objects_pre_work(frame_objects_t* pObjects, double frame_time);
void frame_objects_work(frame_objects_t* pObjects, double frame_time);
void frame_objects_post_work(frame_objects_t* pObjects, double frame_time);
// Dump all object descriptions.
bool frame_objects_dump(frame_objects_t* pObjects, const char* file_name);

#endif // __FRAME_OBJECTS_H
