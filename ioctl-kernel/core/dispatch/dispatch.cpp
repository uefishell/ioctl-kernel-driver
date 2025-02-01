#include "dispatch.hpp"
#include <core/requests/requests.hpp>


namespace dispatch
{
    NTSTATUS ioctl( PDEVICE_OBJECT obj, IRP* irp ) {
        UNREFERENCED_PARAMETER( obj );
        irp->IoStatus.Status = STATUS_SUCCESS;

        const auto stack = IoGetCurrentIrpStackLocation( irp );

        auto buffer = reinterpret_cast< invoke_data* >( irp->AssociatedIrp.SystemBuffer );


        size_t size;
        if ( stack ) {
            switch ( buffer->code )
            {
            case invoke_base:
            {

                if ( request::get_image_base( buffer ) != STATUS_SUCCESS )
                {
                    irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
                }

                break;
            }
            case invoke_dtb:
            {
                if ( request::resolve_dtb( buffer ) != STATUS_SUCCESS )
                {
                    irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
                }

                break;
            }

            case invoke_read:
            {
                if ( request::read_memory( buffer ) != STATUS_SUCCESS )
                {
                    irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
                }

                break;
            }


            default:
            {
                break;
            }
            }
        }

        IofCompleteRequest( irp, 0 );

        return irp->IoStatus.Status;
    }
}