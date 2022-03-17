// frame_api.h
// Created by lulimin on 2019/2/17.

#ifndef __FRAME_API_H
#define __FRAME_API_H

#include "common.h"
#include "uniqid.h"
#include "frame_preset.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

// Frame interface.

#ifdef K_DYNAMIC_LIB
	#define FRAME_API extern "C" K_LIB_IMPORT
#else
	#define FRAME_API
#endif // K_DYNAMIC_LIB

#include "frame_func.h"

// Record log.
#define K_LOG(info) frame_log_string(FRAME_DEFAULT_LOG, info)

#endif // __FRAME_API_H
