#include <dependencies/includes.hpp>
#include <core/driver/driver.hpp>


void pml4e_caching( )
{
	while ( 1 )
	{
		request.dtb = request.resolve_directory_table_base( );
		printf( "directory-table-base: 0x%llx.\n", request.dtb );

		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}

std::int32_t main( )
{
	

	SetConsoleTitleA( "ioctl-usermode" );

	if ( !request.initialize_handle( ) )
	{
		printf( "Failed to create the handle.\n" );
	}

	auto pid = request.get_process_id( L"notepad.exe" ); //FortniteClient-Win64-Shipping

	printf( "Process ID: %d.\n", pid );

	if ( !request.attach( pid ) )
	{
		printf( "Failed to attach to the target process.\n" );
	}

	request.image_base = request.get_image_base( 0 );

	printf( "image-base: 0x%llx.\n", request.image_base );

	std::thread( [ & ]( ) { pml4e_caching( ); } ).detach( );

	
	
	return std::cin.get( ) != EOF;
}