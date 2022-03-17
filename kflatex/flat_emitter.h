// flat_emitter.h
// Created by lulimin on 2021/4/1.

#ifndef __FLAT_EMITTER_H
#define __FLAT_EMITTER_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"

// FlatEmitter

class FlatEmitter : public IObject
{
public:
	FlatEmitter();
	virtual ~FlatEmitter();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

private:
};

#endif // __FLAT_EMITTER_H
