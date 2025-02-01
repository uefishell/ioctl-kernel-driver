#pragma once

#include <dependencies/includes.hpp>

namespace dispatch
{
	NTSTATUS ioctl( PDEVICE_OBJECT, IRP* );
}