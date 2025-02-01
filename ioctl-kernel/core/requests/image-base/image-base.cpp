#include "image-base.hpp"

namespace image_base
{
	NTSTATUS get_image_base( invoke_data* request )
	{

		base_invoke data = { 0 };

		RtlCopyMemory( &data, request->data, sizeof( base_invoke ) );

		if ( !data.pid ) { return STATUS_UNSUCCESSFUL; }

		PEPROCESS process = 0;
		if ( ( PsLookupProcessByProcessId )( ( HANDLE )data.pid, &process ) != STATUS_SUCCESS ) { return STATUS_UNSUCCESSFUL; }

		uintptr_t base = ( uintptr_t )( PsGetProcessSectionBaseAddress )( process );

		reinterpret_cast< base_invoke* > ( request->data )->handle = base;

		return STATUS_SUCCESS;
	}
}