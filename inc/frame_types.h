// frame_types.h
// Created by lulimin on 2019/2/20.

#ifndef __FRAME_TYPES_H
#define __FRAME_TYPES_H

#include "common.h"
#include "frame_mem.h"
#include "arg_data.h"
#include "arg_list.h"
#include "auto_string.h"

typedef TArgData<4, FrameAlloc> FrameArgData;
typedef TArgList<8, 128, FrameAlloc> FrameArgList;
typedef TAutoString<char, 16, TCharTraits<char>, FrameAlloc> FrameString;

#endif // __FRAME_TYPES_H
