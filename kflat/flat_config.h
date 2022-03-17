// flat_config.h
// Created by lulimin on 2021/1/5.

#ifndef __FLAT_CONFIG_H
#define __FLAT_CONFIG_H

#include "../inc/common.h"

class FlatUI;
class FlatWidget;

// Load widget configuration.
FlatWidget* flat_load_config(FlatUI* pFlatUI, const char* file_name, 
	bool design_mode);
// Save widget configuration.
bool flat_save_config(FlatUI* pFlatUI, FlatWidget* pForm,
	const char* file_name);

#endif // __FLAT_CONFIG_H
