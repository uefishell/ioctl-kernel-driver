#include "kernel.hpp"

#define o

#define in_range( x, a, b ) ( x >= a && x <= b ) 
#define get_bits( x ) ( in_range( ( x & ( o( ~0x20 ) ) ), o( 'A' ), o( 'F' ) ) ? ( ( x & ( o( ~0x20 ) ) ) - o( 'A' ) + o( 0xA ) ) : ( in_range( x, o( '0' ), o( '9' ) ) ? x - o( '0' ) : o( 0 ) ) )
#define get_byte( x ) ( get_bits( x[ o( 0 ) ] ) << o( 4 ) | get_bits( x[ o( 1 ) ] ) )

namespace utilities::kernel
{
    namespace pattern {
        uint64_t find_pattern( uint64_t base, size_t range, const char* pattern, const char* mask ) {
            const auto check_mask = [ ]( const char* base, const char* pattern, const char* mask ) -> bool {
                for ( ; *mask; ++base, ++pattern, ++mask ) {
                    if ( *mask == o( 'x' ) && *base != *pattern ) {
                        return o( false );
                    }
                }

                return o( true );
                };

            range = range - strlen( mask );

            for ( size_t i = 0; i < range; ++i ) {
                if ( check_mask( ( const char* )base + i, pattern, mask ) ) {
                    return base + i;
                }
            }

            return o( NULL );
        }

        uint64_t find_pattern( uint64_t base, const char* pattern, const char* mask ) {
            const PIMAGE_NT_HEADERS headers = ( PIMAGE_NT_HEADERS )( base + ( ( PIMAGE_DOS_HEADER )base )->e_lfanew );
            const PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION( headers );

            for ( size_t i = 0; i < headers->FileHeader.NumberOfSections; i++ ) {
                const PIMAGE_SECTION_HEADER section = &sections[ i ];

                if ( section->Characteristics & o( IMAGE_SCN_MEM_EXECUTE ) ) {
                    const auto match = find_pattern( base + section->VirtualAddress, section->Misc.VirtualSize, pattern, mask );

                    if ( match ) {
                        return match;
                    }
                }
            }

            return o( 0 );
        }

        uint64_t find_pattern( uint64_t module_base, const char* pattern ) {
            auto pattern_ = pattern;
            uint64_t first_match = 0;

            if ( !module_base ) {
                return o( 0 );
            }

            const auto nt = reinterpret_cast< IMAGE_NT_HEADERS* >( module_base + reinterpret_cast< IMAGE_DOS_HEADER* >( module_base )->e_lfanew );

            for ( auto current = module_base; current < module_base + nt->OptionalHeader.SizeOfImage; current++ ) {
                if ( !*pattern_ ) {
                    return first_match;
                }

                if ( *( uint8_t* )pattern_ == o( '\?' ) || *( uint8_t* )current == get_byte( pattern_ ) ) {
                    if ( !first_match )
                        first_match = current;

                    if ( !pattern_[ o( 2 ) ] )
                        return first_match;

                    if ( *( uint16_t* )pattern_ == o( '\?\?' ) || *( uint8_t* )pattern_ != o( '\?' ) )
                        pattern_ += o( 3 );

                    else
                        pattern_ += o( 2 );
                }
                else {
                    pattern_ = pattern;
                    first_match = o( 0 );
                }
            }

            return o( 0 );
        }
    }
    void* get_system_information( SYSTEM_INFORMATION_CLASS information_class ) {
        unsigned long size = ( 32 );
        char buffer[ 32 ];

        ( ZwQuerySystemInformation )( information_class, buffer, size, &size );
        void* info = (ExAllocatePoolZero)( NonPagedPool, size, ( 7265746172 ) );

        if ( !info )
            return ( nullptr );

        if ( !NT_SUCCESS( ZwQuerySystemInformation( information_class, info, size, &size ) ) ) {
            ExFreePool( info );
            return ( nullptr );
        }

        return info;
    }


    uintptr_t resolve_address( uintptr_t Instruction, ULONG OffsetOffset, ULONG InstructionSize )
    {
        LONG RipOffset = *( PLONG )( Instruction + OffsetOffset );
        auto ResolvedAddr = (
            Instruction +
            InstructionSize +
            RipOffset );

        return ResolvedAddr;
    }

    uintptr_t get_ntos_base_address( )
    {
        typedef unsigned char uint8_t;
        auto Idt_base = reinterpret_cast< uintptr_t >( KeGetPcr( )->IdtBase );
        auto align_page = *reinterpret_cast< uintptr_t* >( Idt_base + 4 ) >> 0xc << 0xc;

        for ( ; align_page; align_page -= PAGE_SIZE )
        {
            for ( int index = 0; index < PAGE_SIZE - 0x7; index++ )
            {
                auto current_address = static_cast< intptr_t >( align_page ) + index;

                if ( *reinterpret_cast< uint8_t* >( current_address ) == 0x48
                    && *reinterpret_cast< uint8_t* >( current_address + 1 ) == 0x8D
                    && *reinterpret_cast< uint8_t* >( current_address + 2 ) == 0x1D
                    && *reinterpret_cast< uint8_t* >( current_address + 6 ) == 0xFF ) //48 8d 1D ?? ?? ?? FF
                {
                    // rva our virtual address lol
                    auto Ntosbase = resolve_address( current_address, 3, 7 );
                    if ( !( ( UINT64 )Ntosbase & 0xfff ) )
                    {
                        return Ntosbase;
                    }
                }
            }
        }
        return 0;
    }
}