// flat_context.h
// Created by lulimin on 2021/4/21.

#ifndef __FLAT_CONTEXT_H
#define __FLAT_CONTEXT_H

#include "../inc/common.h"
#include "flat_matrix.h"

struct flat_canvas_t;

// Flat scene context.
struct flat_context_t
{
	flat_canvas_t* pCanvas;
	float fCenterX;
	float fCenterY;
	float fBoundLeft;
	float fBoundTop;
	float fBoundRight;
	float fBoundBottom;
	flat_matrix_t fmViewer;
};

// Create scene context.
flat_context_t* flat_context_create();
// Delete scene context.
void flat_context_delete(flat_context_t* pContext);

#endif // __FLAT_CONTEXT_H
