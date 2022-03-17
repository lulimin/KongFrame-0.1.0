// flat_emitter.cpp
// Created by lulimin on 2021/4/1.

#include "flat_emitter.h"

/// \file flat_emitter.cpp
/// \brief Emitter in flat scene.

/// object: FlatEmitter
/// \brief Emitter in flat scene object.
DEF_OBJECT(FlatEmitter, IObject);

// FlatEmitter

FlatEmitter::FlatEmitter()
{
}

FlatEmitter::~FlatEmitter()
{
}

bool FlatEmitter::Startup(const IArgList& args)
{
	return true;
}

bool FlatEmitter::Shutdown()
{
	return true;
}
