#pragma once

#include <dependencies/includes.hpp>


namespace utilities::kernel
{
	namespace pattern {
		uint64_t find_pattern( uint64_t, size_t, const char*, const char* );
		uint64_t find_pattern( uint64_t, const char*, const char* );
		uint64_t find_pattern( uint64_t, const char* );
	}

	void* get_system_information( SYSTEM_INFORMATION_CLASS );
	uint64_t get_module( const char* );

	bool write_to_rw_memory( void*, void*, size_t );

	uintptr_t get_ntos_base_address( );
}