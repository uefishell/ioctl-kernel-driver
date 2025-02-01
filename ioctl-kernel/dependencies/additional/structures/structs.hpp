#pragma once

#include <dependencies/includes.hpp>

namespace globals
{
	inline uintptr_t ntos_image_base = 0;
}

typedef enum _requests{
	invoke_unique,
	invoke_base,
	invoke_read,
	invoke_write,
	invoke_dtb,
}requests, * prequests;

typedef struct _base_invoke {
	uint32_t pid;
	uintptr_t handle;
} base_invoke, * pbase_invoke;

typedef struct _read_invoke {
	uint32_t pid;
	uint64_t directory_table;
	uint64_t address;
	void* buffer;
	size_t size;
} read_invoke, * pread_invoke;

typedef struct _dtb_invoke {
	uint32_t pid;
	uintptr_t dtb;
} dtb_invoke, * pdtb_invoke;


typedef struct _invoke_data{
	uint32_t unique;
	requests code;
	void* data;
} invoke_data, * pinvoke_data;