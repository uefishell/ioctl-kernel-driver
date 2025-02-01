#include <dependencies/includes.hpp>


NTSTATUS io_dispatch( PDEVICE_OBJECT device_obj, PIRP irp ) {
	UNREFERENCED_PARAMETER( device_obj );

	irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest( irp, IO_NO_INCREMENT );

	return irp->IoStatus.Status;
}

auto io_close( PDEVICE_OBJECT device_object, IRP* irp ) -> long
{
	UNREFERENCED_PARAMETER( device_object );

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	( IofCompleteRequest )( irp, IO_NO_INCREMENT );
	return STATUS_SUCCESS;
}

void io_unload( PDRIVER_OBJECT drv_obj ) {
	NTSTATUS status = { };

	UNICODE_STRING link;
	RtlInitUnicodeString( &link, L"\\DosDevices\\{4CF37457-D723-A910-F4AC-56E93D9C2D17}" );

	status = IoDeleteSymbolicLink( &link );

	if ( !NT_SUCCESS( status ) )
		return;

	IoDeleteDevice( drv_obj->DeviceObject );
}

NTSTATUS initialize_driver( PDRIVER_OBJECT drv_obj, PUNICODE_STRING path ) {
	UNREFERENCED_PARAMETER( path );

	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT device_obj = NULL;

	UNICODE_STRING name, link;
	RtlInitUnicodeString( &name, L"\\Device\\{4CF37457-D723-A910-F4AC-56E93D9C2D17}" );
	RtlInitUnicodeString( &link, L"\\DosDevices\\{4CF37457-D723-A910-F4AC-56E93D9C2D17}" );

	status = IoCreateDevice( drv_obj, 0, &name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_obj );
	if ( !NT_SUCCESS( status ) ) {
		return status;
	}

	status = IoCreateSymbolicLink( &link, &name );
	if ( !NT_SUCCESS( status ) ) {
		IoDeleteDevice( device_obj );
		return status;
	}

	for ( int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++ ) {
		drv_obj->MajorFunction[ i ] = &io_dispatch;
	}

	drv_obj->MajorFunction[ IRP_MJ_CREATE ] = &io_close;
	drv_obj->MajorFunction[ IRP_MJ_CLOSE ] = &io_close;
	drv_obj->MajorFunction[ IRP_MJ_DEVICE_CONTROL ] = &dispatch::ioctl;
	drv_obj->DriverUnload = &io_unload;

	

	device_obj->Flags |= DO_BUFFERED_IO;
	device_obj->Flags &= ~DO_DEVICE_INITIALIZING;


	DbgPrintEx( 0, 0, "successfully created driver" );
	return status;
}

NTSTATUS entry( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath ) {
	UNREFERENCED_PARAMETER( DriverObject );
	UNREFERENCED_PARAMETER( RegistryPath );

	return IoCreateDriver( NULL, &initialize_driver );
}