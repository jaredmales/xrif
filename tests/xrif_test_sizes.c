/** \file xrif_test_sizes.c
 * \brief Boundary regressions for checked geometry, codec lengths, and both header versions.
 */

#include "../src/xrif.h"
#include "../src/xrif_size_internal.h"
#include <check.h>
#include <limits.h>
#ifdef _OPENMP
    #include <omp.h>
#endif

/// \cond
/// Construct a small handle without allocating image buffers.
static xrif_t sized_handle( void )
{
    xrif_t handle;
    ck_assert_int_eq( xrif_new( &handle ), XRIF_NOERROR );
    ck_assert_int_eq( xrif_set_size( handle, 3, 5, 1, 2, XRIF_TYPECODE_INT16 ), XRIF_NOERROR );
    return handle;
}

/// Compare complete state including buffer ownership after a rejected operation.
static void unchanged( xrif_t handle, const xrif_handle *before )
{
    ck_assert_int_eq( memcmp( handle, before, sizeof( *handle ) ), 0 );
}
/// \endcond

/// \test Exact wide products and transactional rejection by \ref xrif_set_size.
START_TEST( geometry_boundaries )
{
    xrif_t      h      = sized_handle();
    xrif_handle before = *h;
    ck_assert_int_eq( xrif_set_size( h, 0, 1, 1, 1, XRIF_TYPECODE_INT16 ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
    ck_assert_int_eq( xrif_set_size( h, 1, 1, 1, 1, -1 ), XRIF_ERROR_INVALID_TYPE );
    unchanged( h, &before );
    ck_assert_int_eq( xrif_set_size( h, UINT32_MAX, UINT32_MAX, 1, 1, XRIF_TYPECODE_UINT16 ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
#if SIZE_MAX >= UINT64_MAX
    ck_assert_int_eq( xrif_set_size( h, 65536, 65537, 1, 1, XRIF_TYPECODE_INT16 ), XRIF_NOERROR );
    ck_assert( h->m_raw_size == UINT64_C( 8590065664 ) );
    ck_assert( xrif_min_raw_size( h ) == 0 ); /* Default LZ4 cannot represent this input. */
    ck_assert_int_eq( xrif_allocate( h ), XRIF_ERROR_INVALID_SIZE );
    ck_assert_ptr_null( h->m_raw_buffer );
    h->m_compress_on_raw = 0;
    ck_assert( xrif_min_raw_size( h ) == UINT64_C( 8590065664 ) );
    ck_assert_int_eq( xrif_allocate( h ), XRIF_ERROR_INVALID_SIZE );
    ck_assert_ptr_null( h->m_raw_buffer );
    ck_assert_int_eq( xrif_set_size( h, 65536, 65536, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( h->m_raw_size == UINT64_C( 4294967296 ) );
    ck_assert_int_eq( xrif_set_size( h, 1, 1, 65536, 65536, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( h->m_raw_size == UINT64_C( 4294967296 ) );
    /* UINT64_MAX's factors fit the individual 32-bit axes. No allocation is attempted. */
    ck_assert_int_eq( xrif_set_size( h, UINT32_MAX, 641, 6700417, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( h->m_raw_size == UINT64_MAX );
    before = *h;
    ck_assert_int_eq( xrif_set_size( h, UINT32_MAX, 641, 6700417, 2, XRIF_TYPECODE_UINT8 ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
#else
    ck_assert_int_eq( xrif_set_size( h, 65536, 65537, 1, 1, XRIF_TYPECODE_INT16 ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
#endif
    size_t result = 19;
    ck_assert( !xrif_size_add( SIZE_MAX, 1, &result ) );
    ck_assert( result == 19 );
    ck_assert( !xrif_size_multiply( SIZE_MAX, 2, &result ) );
    ck_assert( result == 19 );
    ck_assert( xrif_size_multiply( SIZE_MAX, 1, &result ) );
    ck_assert( result == SIZE_MAX );
    ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
}
END_TEST

/// \test Padding addition and codec boundaries via minimum-size APIs without large allocations.
START_TEST( method_boundaries )
{
    xrif_t h            = sized_handle();
    h->m_reorder_method = XRIF_REORDER_BYTEPACK_RENIBBLE;
    ck_assert_int_eq( xrif_set_size( h, 1, 1, 1, UINT32_MAX, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
#if SIZE_MAX >= UINT64_MAX
    ck_assert( xrif_min_reordered_size( h ) == UINT64_C( 4294967296 ) );
    h->m_reorder_method = XRIF_REORDER_BITPACK;
    ck_assert( xrif_min_reordered_size( h ) == UINT64_C( 68719476736 ) );
#else
    ck_assert( xrif_min_reordered_size( h ) == 0 );
#endif
    h->m_reorder_method = XRIF_REORDER_NONE;
    ck_assert_int_eq( xrif_set_size( h, LZ4_MAX_INPUT_SIZE, 1, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( xrif_min_compressed_size_lz4( h ) > LZ4_MAX_INPUT_SIZE );
    ck_assert( xrif_min_compressed_size_lz4hc( h ) == xrif_min_compressed_size_lz4( h ) );
    ck_assert_int_eq( xrif_set_size( h, LZ4_MAX_INPUT_SIZE + 1, 1, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( xrif_min_compressed_size_lz4( h ) == 0 );
    ck_assert( xrif_min_compressed_size_lz4hc( h ) == 0 );
    ck_assert_int_eq( xrif_compress_lz4( h ), XRIF_ERROR_INVALID_SIZE );
    ck_assert_int_eq( xrif_set_size( h, INT_MAX, 1, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( xrif_min_compressed_size_fastlz( h ) == 0 );
    ck_assert_int_eq( xrif_set_size( h, 16, 1, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( xrif_min_compressed_size_fastlz( h ) == 66 );
#if SIZE_MAX >= UINT64_MAX
    ck_assert_int_eq( xrif_set_size( h, 65536, 65536, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( xrif_min_compressed_size_zstd( h ) > UINT32_MAX );
    ck_assert( xrif_min_compressed_size_zlib( h ) > UINT32_MAX );
    ck_assert_int_eq( xrif_set_size( h, UINT32_MAX, 641, 6700417, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    ck_assert( xrif_min_compressed_size_zstd( h ) == 0 );
    ck_assert( xrif_min_compressed_size_zlib( h ) == 0 );
    h->m_reorder_method = XRIF_REORDER_BYTEPACK_RENIBBLE;
    ck_assert( xrif_min_reordered_size( h ) == 0 );
#endif
    ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
}
END_TEST

/// \test Size failures preserve owned storage; all transform entry points validate byte capacity.
START_TEST( storage_rejection )
{
    xrif_t h = sized_handle();
    ck_assert_int_eq( xrif_allocate( h ), XRIF_NOERROR );
    xrif_handle before = *h;
    char        byte   = 0;
    ck_assert_int_eq( xrif_set_raw( h, &byte, 1 ), XRIF_ERROR_INSUFFICIENT_SIZE );
    unchanged( h, &before );
    ck_assert_int_eq( xrif_set_reordered( h, &byte, 1 ), XRIF_ERROR_INSUFFICIENT_SIZE );
    unchanged( h, &before );
    ck_assert_int_eq( xrif_set_compressed( h, &byte, 1 ), XRIF_ERROR_INSUFFICIENT_SIZE );
    unchanged( h, &before );
    h->m_reorder_method = -999;
    before              = *h;
    ck_assert_int_eq( xrif_allocate_raw( h ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
    ck_assert_int_eq( xrif_allocate_reordered( h ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
    ck_assert_int_eq( xrif_allocate_compressed( h ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
    h->m_reorder_method = XRIF_REORDER_BYTEPACK;
    ck_assert_int_eq( xrif_set_size( h, 65536, 1024, 1, 1, XRIF_TYPECODE_INT16 ), XRIF_NOERROR );
    xrif_error_t ( *operations[] )( xrif_t ) = { xrif_encode,
                                                 xrif_decode,
                                                 xrif_difference,
                                                 xrif_undifference,
                                                 xrif_difference_previous,
                                                 xrif_undifference_previous,
                                                 xrif_difference_previous0,
                                                 xrif_undifference_previous0,
                                                 xrif_difference_first,
                                                 xrif_undifference_first,
                                                 xrif_difference_pixel0,
                                                 xrif_undifference_pixel0,
                                                 xrif_difference_pixelL,
                                                 xrif_undifference_pixelL,
                                                 xrif_reorder_none,
                                                 xrif_unreorder_none,
                                                 xrif_reorder_bytepack,
                                                 xrif_unreorder_bytepack,
                                                 xrif_reorder_bytepack_renibble,
                                                 xrif_unreorder_bytepack_renibble,
                                                 xrif_reorder_bitpack,
                                                 xrif_unreorder_bitpack };
    for( size_t i = 0; i < sizeof( operations ) / sizeof( operations[0] ); ++i )
        ck_assert_int_eq( operations[i]( h ), XRIF_ERROR_INSUFFICIENT_SIZE );
    ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR ); /* Original F02 failed here under ASan. */
}
END_TEST

/// \test Version-1 golden bytes, unaligned storage, and the full uint64_t payload field.
START_TEST( header_v1 )
{
    xrif_t h = sized_handle(), out = sized_handle();
    h->m_difference_method           = XRIF_DIFFERENCE_NONE;
    h->m_reorder_method              = XRIF_REORDER_NONE;
    h->m_compress_method             = XRIF_COMPRESS_ZSTD;
    h->m_zstd_level                  = -5;
    h->m_compressed_size             = UINT32_C( 0x01020304 );
    const unsigned char expected[56] = { 'x', 'r', 'i', 'f', 1, 0, 0, 0, 56, 0, 0,   0,   3,   0,   0,   0,   5, 0, 0,
                                         0,   1,   0,   0,   0, 2, 0, 0, 0,  4, 0,   255, 255, 255, 255, 120, 0, 0, 0,
                                         0,   0,   4,   3,   2, 1, 0, 0, 0,  0, 251, 255, 255, 255, 0,   0,   0, 0 };
    char                buffer[XRIF_HEADER_SIZE + 2];
    memset( buffer, 0x5a, sizeof( buffer ) );
    ck_assert_int_eq( xrif_write_header_sized( buffer + 1, XRIF_HEADER_SIZE, h ), XRIF_NOERROR );
    ck_assert_int_eq( memcmp( buffer + 1, expected, sizeof( expected ) ), 0 );
    ck_assert( buffer[0] == 0x5a && buffer[sizeof( buffer ) - 1] == 0x5a );
    uint32_t length = 0;
    ck_assert_int_eq( xrif_read_header_sized( out, &length, buffer + 1, XRIF_HEADER_SIZE ), XRIF_NOERROR );
    ck_assert( length == 56 && out->m_raw_size == 60 );
    ck_assert( out->m_zstd_level == -5 && out->m_compressed_size == h->m_compressed_size );
    ck_assert( out->m_difference_method == -1 && out->m_reorder_method == -1 );
#if SIZE_MAX >= UINT64_MAX
    const uint64_t sizes[] = { UINT32_MAX, UINT64_C( 4294967296 ), UINT64_C( 0x0102030405060708 ), UINT64_MAX };
    for( size_t i = 0; i < sizeof( sizes ) / sizeof( sizes[0] ); ++i )
    {
        h->m_compressed_size = sizes[i];
        ck_assert_int_eq( xrif_write_header( buffer + 1, h ), XRIF_NOERROR );
        ck_assert_int_eq( xrif_read_header( out, &length, buffer + 1 ), XRIF_NOERROR );
        ck_assert( out->m_compressed_size == sizes[i] );
    }
    for( size_t i = 40; i < 48; ++i )
        ck_assert( (unsigned char)buffer[1 + i] == 255 );
#else
    memset( buffer + 1 + 40, 255, 8 );
    ck_assert_int_eq( xrif_read_header_sized( out, &length, buffer + 1, XRIF_HEADER_SIZE ), XRIF_ERROR_INVALID_SIZE );
#endif
    ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
    ck_assert_int_eq( xrif_delete( out ), XRIF_NOERROR );
}
END_TEST

/// \test Version-0 decoding preserves native-endian compatibility and reconstructs raw size.
START_TEST( header_v0 )
{
    char  storage[XRIF_HEADER_SIZE_V0 + 1] = { 0 };
    char *header                           = storage + 1;
    memcpy( header, "xrif", 4 );
    const uint32_t fields[] = { 0, 48, 3, 5, 1, 2 };
    for( size_t i = 0; i < 6; ++i )
        memcpy( header + 4 + 4 * i, &fields[i], 4 );
    uint16_t type       = XRIF_TYPECODE_INT16;
    int16_t  method     = -1;
    uint32_t compressed = UINT32_MAX;
    memcpy( header + 28, &type, 2 );
    memcpy( header + 30, &method, 2 );
    memcpy( header + 32, &method, 2 );
    memcpy( header + 34, &method, 2 );
    memcpy( header + 36, &compressed, 4 );
    xrif_t out = sized_handle();
    ck_assert_int_eq( xrif_set_size( out, 1, 1, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
    uint32_t length = 0;
    ck_assert_int_eq( xrif_read_header_sized( out, &length, header, 48 ), XRIF_NOERROR );
    ck_assert( length == 48 && out->m_raw_size == 60 && out->m_data_size == 2 );
    ck_assert( out->m_compressed_size == UINT32_MAX && out->m_compress_method == -1 );
    ck_assert_int_eq( xrif_read_header( out, &length, header ), XRIF_NOERROR );
    ck_assert_int_eq( xrif_delete( out ), XRIF_NOERROR );
}
END_TEST

/// \test Bounds-aware header APIs reject truncation and invalid metadata transactionally.
START_TEST( header_rejection )
{
    xrif_t h = sized_handle();
    ck_assert_int_eq( xrif_allocate( h ), XRIF_NOERROR );
    char header[XRIF_HEADER_SIZE + 4];
    memset( header, 0, sizeof( header ) );
    ck_assert_int_eq( xrif_write_header( header, h ), XRIF_NOERROR );
    xrif_handle before = *h;
    uint32_t    length = 777;
    for( size_t i = 0; i < XRIF_HEADER_SIZE; ++i )
    {
        ck_assert_int_eq( xrif_read_header_sized( h, &length, header, i ), XRIF_ERROR_INSUFFICIENT_SIZE );
        unchanged( h, &before );
        ck_assert( length == 777 );
    }
    char saved[sizeof( header )];
    memcpy( saved, header, sizeof( saved ) );
    ck_assert_int_eq( xrif_write_header_sized( header, 48, h ), XRIF_ERROR_INSUFFICIENT_SIZE );
    ck_assert_int_eq( memcmp( header, saved, sizeof( saved ) ), 0 );
    header[4] = 2;
    ck_assert_int_eq( xrif_read_header_sized( h, &length, header, sizeof( header ) ), XRIF_ERROR_WRONGVERSION );
    unchanged( h, &before );
    header[4] = 1;
    header[8] = 48;
    ck_assert_int_eq( xrif_read_header_sized( h, &length, header, sizeof( header ) ), XRIF_ERROR_BADHEADER );
    unchanged( h, &before );
    header[8] = XRIF_HEADER_SIZE;
    memset( header + 12, 255, 16 ); /* Four UINT32_MAX axes overflow even uint64_t. */
    ck_assert_int_eq( xrif_read_header_sized( h, &length, header, sizeof( header ) ), XRIF_ERROR_INVALID_SIZE );
    unchanged( h, &before );
    ck_assert( length == 777 );
    memcpy( header, saved, sizeof( header ) );
    header[8] = sizeof( header ); /* Optional extension bytes follow fixed version-1 fields. */
    ck_assert_int_eq( xrif_read_header_sized( h, &length, header, sizeof( header ) ), XRIF_NOERROR );
    ck_assert( length == sizeof( header ) );
    ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
}
END_TEST

/// \test Round trips through both buffer modes and all codecs, including zlib window transitions.
START_TEST( codec_roundtrips )
{
    const int methods[] = { XRIF_COMPRESS_NONE,
                            XRIF_COMPRESS_LZ4,
                            XRIF_COMPRESS_LZ4HC,
                            XRIF_COMPRESS_FASTLZ,
                            XRIF_COMPRESS_ZLIB,
                            XRIF_COMPRESS_ZSTD };
    for( size_t method = 0; method < sizeof( methods ) / sizeof( methods[0] ); ++method )
        for( int on_raw = 0; on_raw <= 1; ++on_raw )
            for( size_t size = 1; size <= 259; ++size )
            {
                xrif_t h;
                ck_assert_int_eq( xrif_new( &h ), XRIF_NOERROR );
                h->m_compress_on_raw = on_raw;
                ck_assert_int_eq( xrif_set_size( h, (uint32_t)size, 1, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
                ck_assert_int_eq( xrif_configure( h, XRIF_DIFFERENCE_NONE, XRIF_REORDER_NONE, methods[method] ),
                                  XRIF_NOERROR );
                ck_assert_int_eq( xrif_allocate( h ), XRIF_NOERROR );
                unsigned char original[259];
                for( size_t i = 0; i < size; ++i )
                    original[i] = (unsigned char)( i * 37 + ( i / 13 ) );
                memcpy( h->m_raw_buffer, original, size );
                ck_assert_int_eq( xrif_encode( h ), XRIF_NOERROR );
                ck_assert_int_eq( xrif_set_compress_direction( h, XRIF_DIRECTION_DECOMPRESS ), XRIF_NOERROR );
                ck_assert_int_eq( xrif_decode( h ), XRIF_NOERROR );
                ck_assert_int_eq( memcmp( h->m_raw_buffer, original, size ), 0 );
                ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
            }
}
END_TEST

/// \test Tiny FastLZ inputs retain the existing literal representation for both levels.
START_TEST( fastlz_literals )
{
    for( int level = 1; level <= 2; ++level )
        for( uint32_t size = 1; size < 16; ++size )
        {
            xrif_t h = sized_handle();
            ck_assert_int_eq( xrif_set_size( h, size, 1, 1, 1, XRIF_TYPECODE_UINT8 ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_configure( h, -1, -1, XRIF_COMPRESS_FASTLZ ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_set_fastlz_level( h, level ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_allocate( h ), XRIF_NOERROR );
            unsigned char original[15];
            for( size_t i = 0; i < size; ++i )
                original[i] = (unsigned char)( i + 42 );
            memcpy( h->m_raw_buffer, original, size );
            ck_assert_int_eq( xrif_encode( h ), XRIF_NOERROR );
            ck_assert( h->m_compressed_size == size + 1 );
            ck_assert( (unsigned char)h->m_raw_buffer[0] == ( size - 1 ) + ( level == 2 ? 32 : 0 ) );
            ck_assert_int_eq( memcmp( h->m_raw_buffer + 1, original, size ), 0 );
            ck_assert_int_eq( xrif_decode( h ), XRIF_NOERROR );
            ck_assert_int_eq( memcmp( h->m_raw_buffer, original, size ), 0 );
            ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
        }
}
END_TEST

/// \test Payload source capacities are checked before every decompressor reads data.
START_TEST( decoder_capacities )
{
    const int methods[] = { -1, 100, 102, 110, 120, 130 };
    for( size_t i = 0; i < sizeof( methods ) / sizeof( methods[0] ); ++i )
        for( int on_raw = 0; on_raw <= 1; ++on_raw )
        {
            xrif_t h             = sized_handle();
            h->m_compress_on_raw = on_raw;
            ck_assert_int_eq( xrif_configure( h, -1, -1, methods[i] ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_allocate( h ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_set_compress_direction( h, XRIF_DIRECTION_DECOMPRESS ), XRIF_NOERROR );
            h->m_compressed_size = ( on_raw ? h->m_raw_buffer_size : h->m_compressed_buffer_size ) + 1;
            ck_assert_int_eq( xrif_decompress( h ), XRIF_ERROR_INSUFFICIENT_SIZE );
            if( methods[i] == XRIF_COMPRESS_NONE )
            {
                h->m_compressed_size = 1;
                ck_assert_int_eq( xrif_decompress( h ), XRIF_ERROR_INVALID_SIZE );
                ck_assert_int_eq( xrif_decode( h ), XRIF_ERROR_INVALID_SIZE );
            }
            ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
        }
}
END_TEST

/// \test Header-driven decoding transfers codec parameters and sizes to a fresh handle.
START_TEST( header_payload_roundtrips )
{
    const int methods[] = { -1, 100, 102, 110, 120, 130 };
    for( size_t i = 0; i < sizeof( methods ) / sizeof( methods[0] ); ++i )
        for( int on_raw = 0; on_raw <= 1; ++on_raw )
        {
            xrif_t input = sized_handle(), output = sized_handle();
            input->m_compress_on_raw  = on_raw;
            output->m_compress_on_raw = on_raw;
            ck_assert_int_eq( xrif_set_compress_method( input, methods[i] ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_allocate( input ), XRIF_NOERROR );
            int16_t original[30];
            for( size_t j = 0; j < 30; ++j )
                original[j] = (int16_t)( j * 123 - 456 );
            memcpy( input->m_raw_buffer, original, sizeof( original ) );
            ck_assert_int_eq( xrif_encode( input ), XRIF_NOERROR );
            char header[XRIF_HEADER_SIZE];
            ck_assert_int_eq( xrif_write_header( header, input ), XRIF_NOERROR );
            uint32_t length;
            ck_assert_int_eq( xrif_read_header_sized( output, &length, header, sizeof( header ) ), XRIF_NOERROR );
            ck_assert( output->m_raw_size == sizeof( original ) );
            ck_assert_int_eq( xrif_set_compress_direction( output, XRIF_DIRECTION_DECOMPRESS ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_allocate( output ), XRIF_NOERROR );
            memcpy( on_raw ? output->m_raw_buffer : output->m_compressed_buffer,
                    on_raw ? input->m_raw_buffer : input->m_compressed_buffer,
                    input->m_compressed_size );
            ck_assert_int_eq( xrif_decode( output ), XRIF_NOERROR );
            ck_assert_int_eq( memcmp( output->m_raw_buffer, original, sizeof( original ) ), 0 );
            ck_assert_int_eq( xrif_delete( input ), XRIF_NOERROR );
            ck_assert_int_eq( xrif_delete( output ), XRIF_NOERROR );
        }
}
END_TEST

#ifdef _OPENMP
/// \test Wide traversal counters retain lossless behavior with four OpenMP workers on independent loops.
START_TEST( parallel_transforms )
{
    omp_set_dynamic( 0 );
    omp_set_num_threads( 4 );
    const int methods[] = {
        XRIF_DIFFERENCE_PREVIOUS0, XRIF_DIFFERENCE_PREVIOUS, XRIF_DIFFERENCE_FIRST, XRIF_DIFFERENCE_PIXELL };
    for( size_t i = 0; i < sizeof( methods ) / sizeof( methods[0] ); ++i )
    {
        xrif_t h = sized_handle();
        ck_assert_int_eq( xrif_set_size( h, 127, 17, 2, 3, XRIF_TYPECODE_INT16 ), XRIF_NOERROR );
        ck_assert_int_eq( xrif_configure( h, methods[i], XRIF_REORDER_BYTEPACK, XRIF_COMPRESS_NONE ), XRIF_NOERROR );
        ck_assert_int_eq( xrif_allocate( h ), XRIF_NOERROR );
        char *original = malloc( h->m_raw_size );
        ck_assert_ptr_nonnull( original );
        for( size_t j = 0; j < h->m_raw_size; ++j )
            original[j] = (char)( j * 37 );
        memcpy( h->m_raw_buffer, original, h->m_raw_size );
        h->m_omp_parallel = 1;
        ck_assert_int_eq( xrif_encode( h ), XRIF_NOERROR );
        ck_assert_int_eq( xrif_decode( h ), XRIF_NOERROR );
        ck_assert_int_eq( memcmp( h->m_raw_buffer, original, h->m_raw_size ), 0 );
        free( original );
        ck_assert_int_eq( xrif_delete( h ), XRIF_NOERROR );
    }
}
END_TEST
#endif

/// Run focused boundary tests; all large values are metadata-only.
int main( void )
{
    Suite *suite = suite_create( "checked_sizes_and_headers" );
    TCase *tests = tcase_create( "boundaries" );
    tcase_set_timeout( tests, 60 );
    tcase_add_test( tests, geometry_boundaries );
    tcase_add_test( tests, method_boundaries );
    tcase_add_test( tests, storage_rejection );
    tcase_add_test( tests, header_v1 );
    tcase_add_test( tests, header_v0 );
    tcase_add_test( tests, header_rejection );
    tcase_add_test( tests, codec_roundtrips );
    tcase_add_test( tests, fastlz_literals );
    tcase_add_test( tests, decoder_capacities );
    tcase_add_test( tests, header_payload_roundtrips );
#ifdef _OPENMP
    tcase_add_test( tests, parallel_transforms );
#endif
    suite_add_tcase( suite, tests );
    SRunner *runner = srunner_create( suite );
    srunner_run_all( runner, CK_NORMAL );
    int failures = srunner_ntests_failed( runner );
    srunner_free( runner );
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
