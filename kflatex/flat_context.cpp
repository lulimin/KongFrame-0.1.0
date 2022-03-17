// flat_context.cpp
// Created by lulimin on 2021/4/21.

#include "flat_context.h"
#include "../inc/frame_mem.h"

// Create scene context.
flat_context_t* flat_context_create()
{
	flat_context_t* pContext = K_NEW(flat_context_t);

	pContext->pCanvas = NULL;
	pContext->fCenterX = 0.0F;
	pContext->fCenterY = 0.0F;
	pContext->fBoundLeft = -100.0F;
	pContext->fBoundTop = -100.0F;
	pContext->fBoundRight = 100.0F;
	pContext->fBoundBottom = 100.0F;
	flat_matrix_set_identity(&pContext->fmViewer);
	return pContext;
}

// Delete scene context.
void flat_context_delete(flat_context_t* pContext)
{
	Assert(pContext != NULL);

	K_DELETE(pContext);
}
