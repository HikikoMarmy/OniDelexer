#pragma once

#include <cinttypes>

inline bool is_little_endian()
{
	// Isolate the end byte, and figure out of our machine is using it as high or low.
	return std::uint8_t( std::uint16_t( 1 ) ) & 1;
}

inline std::uint16_t byteswap_16( std::uint16_t x )
{
	return	( ( x & 0x00FF ) << 8 ) |
		( ( x & 0xFF00 ) >> 8 );
}

inline std::uint32_t byteswap_32( std::uint32_t x )
{
	return	( ( x & 0x000000FF ) << 24 ) |
		( ( x & 0x0000FF00 ) << 8 ) |
		( ( x & 0x00FF0000 ) >> 8 ) |
		( ( x & 0xFF000000 ) >> 24 );
}

template< typename T >
// Will byteswap 16, 32, and 64 bit types. All others will be returned as is.
inline T byteswap( T x )
{
	if( is_little_endian() )
	{
		switch( sizeof( T ) )
		{
			case sizeof( std::uint16_t ): x = byteswap_16( x ); break;
			case sizeof( std::uint32_t ): x = byteswap_32( x );	break;
			default: break;
		}
	}

	return x;
}
