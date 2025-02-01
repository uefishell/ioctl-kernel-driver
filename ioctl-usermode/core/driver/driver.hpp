#pragma once

#include <dependencies/includes.hpp>

#define device_name "\\\\.\\{4CF37457-D723-A910-F4AC-56E93D9C2D17}"

namespace driver
{
	class communicate_t
	{

		typedef enum _requests {
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

		typedef struct _invoke_data {
			uint32_t unique;
			requests code;
			void* data;
		} invoke_data, * pinvoke_data;

		std::int32_t m_pid = 0;
		void* m_handle = 0;

	public:

		std::uintptr_t image_base = 0;
		std::uintptr_t dtb = 0;

		[[nodiscard]] bool send_cmd( void*, requests );
		[[nodiscard]] bool initialize_handle( );
		[[nodiscard]] bool attach( int );
		[[nodiscard]] uint32_t get_process_id( const LPCWSTR );
		[[nodiscard]] const std::uintptr_t get_image_base( const char* );
		[[nodiscard]] const std::uintptr_t resolve_directory_table_base( );

		[[nodiscard]] bool read_physical( const uintptr_t address, void* buffer, const size_t size );
		template <typename t>
		[[nodiscard]] auto read( const uintptr_t address ) -> t
		{
			t response{};
			read_physical( address, &response, sizeof( t ) );
			return response;
		}

	};
}

inline driver::communicate_t request;