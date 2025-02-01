#pragma once

#include <dependencies/includes.hpp>

namespace directory_table_base
{
	NTSTATUS resolve_dtb( invoke_data* );
	
	namespace pml4
	{
		PVOID split_memory( PVOID, SIZE_T, const void*, SIZE_T );

		inline void* g_mmonp_MmPfnDatabase;

		static NTSTATUS InitializeMmPfnDatabase( );

		uintptr_t dirbase_from_base_address( void* );
	}
}