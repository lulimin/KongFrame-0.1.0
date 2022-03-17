// i_renderable.h
// Created by lulimin on 2020/11/12.

#ifndef __I_RENDERABLE_H
#define __I_RENDERABLE_H

#include "../inc/common.h"
#include "../inc/frame_def.h"

// IRenderable

class IRenderable : public IObject
{
public:
	// Update states per frame.
	virtual void Update(float frame_time) = 0;
	// Render graphics.
	virtual void Render(float frame_time) = 0;
	// Load resource.
	virtual bool Load() { return false; }
	// Get whether loading succeed.
	virtual bool GetLoadSucceed() { return false; }
	// Get whether loading ended.
	virtual bool GetLoadEnded() { return false; }
};

#endif // __I_RENDERABLE_H
