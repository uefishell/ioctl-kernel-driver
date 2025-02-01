#include <dependencies/includes.hpp>

#include "driver.hpp"

auto driver::communicate_t::get_process_id( const LPCWSTR process_name ) -> uint32_t
{
	HANDLE handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
	DWORD procID = NULL;

	if ( handle == INVALID_HANDLE_VALUE )
		return procID;

	PROCESSENTRY32W entry = { 0 };
	entry.dwSize = sizeof( PROCESSENTRY32W );

	if ( Process32FirstW( handle, &entry ) ) {
		if ( !_wcsicmp( process_name, entry.szExeFile ) )
		{
			procID = entry.th32ProcessID;
		}
		else while ( Process32NextW( handle, &entry ) ) {
			if ( !_wcsicmp( process_name, entry.szExeFile ) ) {
				procID = entry.th32ProcessID;
			}
		}
	}

	CloseHandle( handle );
	return procID;
}

auto driver::communicate_t::send_cmd( void* data, requests code ) -> bool
{
	if ( !data || !code )
	{
		return false;
	}


	IO_STATUS_BLOCK block;
	invoke_data request{ 0 };

	request.unique = requests::invoke_unique;
	request.data = data;
	request.code = code;

	auto result = direct_device_control( this->m_handle, nullptr, nullptr, nullptr, &block, 0, &request, sizeof( request ), &request, sizeof( request ) );

	return result;
}

auto driver::communicate_t::initialize_handle( ) -> bool
{
	this->m_handle = CreateFileA( device_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
	if ( this->m_handle != INVALID_HANDLE_VALUE ) {
		return true;
	}
	return false;
}

auto driver::communicate_t::attach(
	int a_pid ) -> bool
{
	if ( !a_pid )
		return false;

	this->m_pid = a_pid;

	return true;
}


auto driver::communicate_t::get_image_base( const char* module_name ) -> const std::uintptr_t
{
	base_invoke data{ 0 };

	data.pid = this->m_pid;
	data.handle = 0;

	if ( !this->send_cmd( &data, invoke_base ) ) { }

	return data.handle;
}

auto driver::communicate_t::read_physical( const std::uintptr_t address, void* buffer, const std::size_t size ) -> bool
{
	read_invoke data{ 0 };

	data.pid = this->m_pid;
	data.directory_table = this->dtb;
	data.address = address;
	data.buffer = buffer;
	data.size = size;

	auto result =
		this->send_cmd(
			&data,
			invoke_read );

	return result;
}

auto driver::communicate_t::resolve_directory_table_base( ) -> const std::uintptr_t
{
	dtb_invoke data{ 0 };

	data.pid = this->m_pid;
	data.dtb = 0;

	if ( !this->send_cmd( &data, invoke_dtb ) ) { }

	return data.dtb;
}


