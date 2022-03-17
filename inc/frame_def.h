// frame_def.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_DEF_H
#define __FRAME_DEF_H

#include "common.h"
#include "frame_api.h"
#include "frame_mem.h"
#include "i_arg_data.h"
#include "i_arg_list.h"
#include "i_object.h"
#include "i_object_desc.h"
#include "i_object_hook.h"
#include "i_object_creator.h"
#include "i_service.h"
#include "i_service_creator.h"
#include "data_type.h"
#include "arg_reader.h"
#include "arg_writer.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

// Component module definition interface.

#ifndef K_DYNAMIC_LIB

// Link service creators.
inline void LinkServiceCreator(IServiceCreator** pLink, IServiceCreator* pSC)
{
	pSC->SetNext(*pLink);
	*pLink = pSC;
}

// Link object creators.
inline void LinkObjectCreator(IObjectCreator** pLink, IObjectCreator* pOC)
{
	pOC->SetNext(*pLink);
	*pLink = pOC;
}

#ifndef LINK_SERVICE
#define LINK_SERVICE(lnk, cls) extern IServiceCreator* cls##_ServiceCreator(); \
	LinkServiceCreator(lnk, cls##_ServiceCreator());
#endif // !LINK_SERVICE

#ifndef LINK_OBJECT
#define LINK_OBJECT(lnk, cls) extern IObjectCreator* cls##_ObjectCreator(); \
	LinkObjectCreator(lnk, cls##_ObjectCreator());
#endif // !LINK_OBJECT 

#endif // !K_DYNAMIC_LIB

#ifdef K_DYNAMIC_LIB

// Frame version.
#define K_FRAME_VERSION 0x100

// Service creator link.
extern IServiceCreator* g_pServiceCreator;
// Object creator link.
extern IObjectCreator* g_pObjectCreator;

// Define component module.
#define DEF_MODULE \
IServiceCreator* g_pServiceCreator = NULL; \
IObjectCreator* g_pObjectCreator = NULL; \
extern "C" K_LIB_EXPORT \
int k_module_api(IServiceCreator** ppSC, IObjectCreator** ppOC) \
{ \
	*ppSC = g_pServiceCreator; \
	*ppOC = g_pObjectCreator; \
	return K_FRAME_VERSION; \
} \
DEFINE_ASSERTION_FAIL;

#else

#define DEF_MODULE

#endif // K_DYNAMIC_LIB

#ifdef K_DYNAMIC_LIB

// Define service.
// cls: Service class name.
#define DEF_SERVICE(cls) \
class cls##ServiceCreator : public IServiceCreator \
 { \
public: \
	cls##ServiceCreator(IServiceCreator** pCreator) \
	: IServiceCreator(*pCreator) { *pCreator = this; } \
	virtual ~cls##ServiceCreator() {} \
	virtual const char* GetName() { return #cls; } \
	virtual IService* Create() { return K_NEW(cls); } \
	virtual void Destroy(IService* p) { K_DELETE((cls*)p); } \
}; \
cls##ServiceCreator cls##_ServiceCreator(&g_pServiceCreator);

#else

#define DEF_SERVICE(cls) \
class cls##ServiceCreator : public IServiceCreator \
 { \
public: \
	cls##ServiceCreator() {} \
	virtual ~cls##ServiceCreator() {} \
	virtual const char* GetName() { return #cls; } \
	virtual IService* Create() { return K_NEW(cls); } \
	virtual void Destroy(IService* p) { K_DELETE((cls*)p); } \
}; \
IServiceCreator* cls##_ServiceCreator() \
{ \
	static cls##ServiceCreator s_single; \
	return &s_single; \
}

#endif // K_DYNAMIC_LIB

#ifdef K_DYNAMIC_LIB

#define RAW_OBJECT_CREATOR(cls) \
cls##ObjectCreator* cls##_ObjectCreator(IObjectCreator** pCreator) \
{ \
	static cls##ObjectCreator s_single(pCreator); \
	return &s_single; \
} \
struct cls##ObjectCreator_Stub \
{ \
	cls##ObjectCreator_Stub(IObjectCreator** pCreator) \
	{ \
		cls##_ObjectCreator(pCreator); \
	} \
} cls##ObjectCreator_stub_single(&g_pObjectCreator);

#else

#define RAW_OBJECT_CREATOR(cls) \
IObjectCreator* cls##_ObjectCreator() \
{ \
	static cls##ObjectCreator s_single; \
	return &s_single; \
}

#endif // K_DYNAMIC_LIB

#ifdef K_DYNAMIC_LIB

// Define abstract object class, derive from only.
// cls: Object class name.
// parent: Parent object class name.
#define DEF_BASE_OBJECT(cls, parent) \
class cls##ObjectCreator : public IObjectCreator \
{ \
public: \
	cls##ObjectCreator(IObjectCreator** pCreator) \
	: IObjectCreator(*pCreator) { *pCreator = this; } \
	virtual ~cls##ObjectCreator() {} \
	virtual bool IsBaseClass() { return true; } \
	virtual const char* GetParent() \
		{ parent::FrameCheckValidate(); return #parent; } \
	virtual const char* GetName() { return #cls; } \
	virtual IObject* Create() { return NULL; } \
	virtual void Destroy(IObject* p) {} \
}; \
RAW_OBJECT_CREATOR(cls);

#else

#define DEF_BASE_OBJECT(cls, parent) \
class cls##ObjectCreator : public IObjectCreator \
{ \
public: \
	cls##ObjectCreator() {} \
	virtual ~cls##ObjectCreator() {} \
	virtual bool IsBaseClass() { return true; } \
	virtual const char* GetParent() \
		{ parent::FrameCheckValidate(); return #parent; } \
	virtual const char* GetName() { return #cls; } \
	virtual IObject* Create() { return NULL; } \
	virtual void Destroy(IObject* p) {} \
}; \
RAW_OBJECT_CREATOR(cls);

#endif // K_DYNAMIC_LIB

#ifdef K_DYNAMIC_LIB

// Define object class.
// cls: Object class name.
// parent: Parent object class name.
#define DEF_OBJECT(cls, parent) \
class cls##ObjectCreator : public IObjectCreator \
 { \
public: \
	cls##ObjectCreator(IObjectCreator** pCreator) \
	: IObjectCreator(*pCreator) { *pCreator = this; } \
	virtual ~cls##ObjectCreator() {} \
	virtual bool IsBaseClass() { return false; } \
	virtual const char* GetParent() \
		{ parent::FrameCheckValidate(); return #parent; } \
	virtual const char* GetName() { return #cls; } \
	virtual IObject* Create() { return K_NEW(cls); } \
	virtual void Destroy(IObject* p) { K_DELETE((cls*)p); } \
}; \
RAW_OBJECT_CREATOR(cls);

#else

#define DEF_OBJECT(cls, parent) \
class cls##ObjectCreator : public IObjectCreator \
{ \
public: \
	cls##ObjectCreator() {} \
	virtual ~cls##ObjectCreator() {} \
	virtual bool IsBaseClass() { return false; } \
	virtual const char* GetParent() \
		{ parent::FrameCheckValidate(); return #parent; } \
	virtual const char* GetName() { return #cls; } \
	virtual IObject* Create() { return K_NEW(cls); } \
	virtual void Destroy(IObject* p) { K_DELETE((cls*)p); } \
}; \
RAW_OBJECT_CREATOR(cls);

#endif // K_DYNAMIC_LIB

#ifdef K_DYNAMIC_LIB
	#define OBJECT_CREATOR_ARG (&g_pObjectCreator)
#else
	#define OBJECT_CREATOR_ARG
#endif // K_DYNAMIC_LIB

// Define readonly object property.
// type: Property type.
// cls: Object class name.
// prop: Property name.
// getf: Read property function of class.
#define DEF_READ_F(type, cls, prop, getf) \
void cls##prop##_reader(IObject* obj, IArgData* v) \
{ \
	TArgWriter<type>::Set(v, ((cls*)obj)->getf()); \
} \
struct cls##prop##_object_prop : public object_property_t \
{ \
	cls##prop##_object_prop() \
	{ \
		IObjectCreator* pCreator = cls##_ObjectCreator(OBJECT_CREATOR_ARG); \
		pName = #prop; \
		nType = TDataType<type>::Type; \
		pReadFunc = (void*)(cls##prop##_reader); \
		pWriteFunc = NULL; \
		pNext = pCreator->GetPropertyLink(); \
		pCreator->SetPropertyLink(this); \
	} \
}; \
cls##prop##_object_prop g_##cls##prop##_object_prop;

#define DEF_READ(type, cls, prop) DEF_READ_F(type, cls, prop, Get##prop)

// Define read and write object property.
// type: Property type.
// cls: Object class name.
// prop: Property name.
// getf: Read property function of class.
// setf: Write property function of class.
#define DEF_PROP_RW(type, cls, prop, getf, setf) \
void cls##prop##_reader(IObject* obj, IArgData* v) \
{ \
	TArgWriter<type>::Set(v, ((cls*)obj)->getf()); \
} \
void cls##prop##_writer(IObject* obj, const IArgData& v) \
{ \
	((cls*)obj)->setf(TArgReader<type>::Get(v)); \
} \
struct cls##prop##_object_prop : public object_property_t \
{ \
	cls##prop##_object_prop() \
	{ \
		IObjectCreator* pCreator = cls##_ObjectCreator(OBJECT_CREATOR_ARG); \
		pName = #prop; \
		nType = TDataType<type>::Type; \
		pReadFunc = (void*)(cls##prop##_reader); \
		pWriteFunc = (void*)(cls##prop##_writer); \
		pNext = pCreator->GetPropertyLink(); \
		pCreator->SetPropertyLink(this); \
	} \
}; \
cls##prop##_object_prop g_##cls##prop##_object_prop;

#define DEF_PROP(type, cls, prop) \
	DEF_PROP_RW(type, cls, prop, Get##prop, Set##prop)

// Get object method parameter.
#define RAW_GET_ARG(type, args, index) TArgReader<type>::Get(args, index)

// Object method interface function.
#define RAW_OBJECT_METHOD(cls, func, result_array) \
struct cls##func##_object_method : public object_method_t \
{ \
	cls##func##_object_method() \
	{ \
		IObjectCreator* pCreator = cls##_ObjectCreator(OBJECT_CREATOR_ARG); \
		pName = #func; \
		pMethodFunc = (void*)(cls##func##_method); \
		bResultArray = result_array; \
		pNext = pCreator->GetMethodLink(); \
		pCreator->SetMethodLink(this); \
	} \
}; \
cls##func##_object_method g_##cls##func##_object_method;

// Define object function return multiple parameters.
// cls: Object class name.
// func: Method function name.
#define DEF_FUNC_M(cls, func) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	((cls*)obj)->func(args, res); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

// Define object function return array.
// cls: Object class name.
// func: Method function name.
#define DEF_FUNC_A(cls, func) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	((cls*)obj)->func(args, res); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, true)

// tr: Return value type.
// cls: Object class name.
// func: Method function name.
#define DEF_FUNC_0(tr, cls, func) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 0) { return false; } \
	*res << ((cls*)obj)->func(); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

// tr: Return value type.
// cls: Object class name.
// func: Method function name.
// tN: Parameter type.
#define DEF_FUNC_1(tr, cls, func, t0) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 1) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_2(tr, cls, func, t0, t1) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 2) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_3(tr, cls, func, t0, t1, t2) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 3) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_4(tr, cls, func, t0, t1, t2, t3) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 4) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_5(tr, cls, func, t0, t1, t2, t3, t4) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 5) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_6(tr, cls, func, t0, t1, t2, t3, t4, t5) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 6) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_7(tr, cls, func, t0, t1, t2, t3, t4, t5, t6) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 7) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5), \
		RAW_GET_ARG(t6, args, 6)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_8(tr, cls, func, t0, t1, t2, t3, t4, t5, t6, t7) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 8) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5), \
		RAW_GET_ARG(t6, args, 6), \
		RAW_GET_ARG(t7, args, 7)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

#define DEF_FUNC_9(tr, cls, func, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
bool cls##func##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 9) { return false; } \
	*res << ((cls*)obj)->func( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5), \
		RAW_GET_ARG(t6, args, 6), \
		RAW_GET_ARG(t7, args, 7), \
		RAW_GET_ARG(t8, args, 8)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, func, false)

// cls: Object class name.
// proc: Method procedure name.
#define DEF_PROC_0(cls, proc) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 0) { return false; } \
	((cls*)obj)->proc(); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

// cls: Object class name.
// proc: Method procedure name.
// tN: Parameter type.
#define DEF_PROC_1(cls, proc, t0) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 1) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DEF_PROC_2(cls, proc, t0, t1) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 2) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DECLARE_METHOD_VOID_3(cls, proc, t0, t1, t2) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 3) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DEF_PROC_4(cls, proc, t0, t1, t2, t3) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 4) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DEF_PROC_5(cls, proc, t0, t1, t2, t3, t4) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 5) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DEF_PROC_6(cls, proc, t0, t1, t2, t3, t4, t5) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 6) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DEF_PROC_7(cls, proc, t0, t1, t2, t3, t4, t5, t6) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 7) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5), \
		RAW_GET_ARG(t6, args, 6)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DEF_PROC_8(cls, proc, t0, t1, t2, t3, t4, t5, t6, t7) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 8) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5), \
		RAW_GET_ARG(t6, args, 6), \
		RAW_GET_ARG(t7, args, 7)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#define DEF_PROC_9(cls, proc, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
bool cls##proc##_method(IObject* obj, const IArgList& args, IArgList* res) \
{ \
	if (args.GetCount() != 9) { return false; } \
	((cls*)obj)->proc( \
		RAW_GET_ARG(t0, args, 0), \
		RAW_GET_ARG(t1, args, 1), \
		RAW_GET_ARG(t2, args, 2), \
		RAW_GET_ARG(t3, args, 3), \
		RAW_GET_ARG(t4, args, 4), \
		RAW_GET_ARG(t5, args, 5), \
		RAW_GET_ARG(t6, args, 6), \
		RAW_GET_ARG(t7, args, 7), \
		RAW_GET_ARG(t8, args, 8)); \
	return true; \
} \
RAW_OBJECT_METHOD(cls, proc, false)

#endif // __FRAME_DEF_H
