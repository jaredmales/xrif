/** \file xrif_size_internal.h
 * \brief Private checked geometry and buffer validation for the C implementation.
 */

#ifndef xrif_xrif_size_internal_h
#define xrif_xrif_size_internal_h

#include "xrif.h"
#include <limits.h>
#include <stdint.h>

/// Derived counts in pixels and bytes, all representable by both uint64_t and size_t.
typedef struct
{
    size_t m_element_bytes;
    size_t m_plane_pixels;
    size_t m_frame_pixels;
    size_t m_total_pixels;
    size_t m_frame_bytes;
    size_t m_raw_bytes;
} xrif_geometry;

/// Multiply without exceeding the format's 64-bit or the host's addressable size.
static inline int xrif_size_multiply( size_t a, size_t b, size_t *result )
{
    if( a > UINT64_MAX || b > UINT64_MAX || ( b != 0 && ( a > SIZE_MAX / b || a > UINT64_MAX / b ) ) )
    {
        return 0;
    }
    *result = a * b;
    return 1;
}

/// Add without exceeding the format's 64-bit or the host's addressable size.
static inline int xrif_size_add( size_t a, size_t b, size_t *result )
{
    if( a > UINT64_MAX || b > UINT64_MAX || a > SIZE_MAX - b || a > UINT64_MAX - b )
    {
        return 0;
    }
    *result = a + b;
    return 1;
}

/// Validate a shape without allocating or depending on the chosen codec.
static inline xrif_error_t xrif_geometry_make( xrif_geometry   *geometry,
                                               xrif_dimension_t width,
                                               xrif_dimension_t height,
                                               xrif_dimension_t depth,
                                               xrif_dimension_t frames,
                                               xrif_int_t       type )
{
    if( width == 0 || height == 0 || depth == 0 || frames == 0 )
    {
        return XRIF_ERROR_INVALID_SIZE;
    }
    geometry->m_element_bytes = xrif_typesize( type );
    if( geometry->m_element_bytes == 0 )
    {
        return XRIF_ERROR_INVALID_TYPE;
    }
    if( !xrif_size_multiply( width, height, &geometry->m_plane_pixels ) ||
        !xrif_size_multiply( geometry->m_plane_pixels, depth, &geometry->m_frame_pixels ) ||
        !xrif_size_multiply( geometry->m_frame_pixels, frames, &geometry->m_total_pixels ) ||
        !xrif_size_multiply( geometry->m_frame_pixels, geometry->m_element_bytes, &geometry->m_frame_bytes ) ||
        !xrif_size_multiply( geometry->m_total_pixels, geometry->m_element_bytes, &geometry->m_raw_bytes ) )
    {
        return XRIF_ERROR_INVALID_SIZE;
    }
    return XRIF_NOERROR;
}

/// Validate current public metadata, including cached sizes used by transforms.
static inline xrif_error_t xrif_geometry_get( xrif_t handle, xrif_geometry *geometry )
{
    if( handle == NULL )
    {
        return XRIF_ERROR_NULLPTR;
    }
    if( !handle->m_width || !handle->m_height || !handle->m_depth || !handle->m_frames || !handle->m_type_code )
    {
        return XRIF_ERROR_NOT_SETUP;
    }
    xrif_error_t rv = xrif_geometry_make(
        geometry, handle->m_width, handle->m_height, handle->m_depth, handle->m_frames, handle->m_type_code );
    if( rv != XRIF_NOERROR )
    {
        return rv;
    }
    if( handle->m_data_size != geometry->m_element_bytes || handle->m_raw_size != geometry->m_raw_bytes )
    {
        return XRIF_ERROR_INVALID_SIZE;
    }
    return XRIF_NOERROR;
}

/// Compute a particular reorder method's extent independently of the configured method.
static inline size_t xrif_reordered_bytes( const xrif_geometry *geometry, size_t frames, int method )
{
    if( method == XRIF_REORDER_NONE || method == XRIF_REORDER_BYTEPACK )
    {
        return geometry->m_raw_bytes;
    }
    if( method == XRIF_REORDER_BYTEPACK_RENIBBLE || method == XRIF_REORDER_BITPACK )
    {
        size_t frame_bytes = geometry->m_frame_bytes;
        size_t padded_frames, bytes;
        if( method == XRIF_REORDER_BITPACK && frame_bytes < 16 )
        {
            frame_bytes = 16;
        }
        if( xrif_size_add( frames, 1, &padded_frames ) && xrif_size_multiply( frame_bytes, padded_frames, &bytes ) )
        {
            return bytes;
        }
    }
    return 0;
}

/// Validate the byte extent before a raw-data transform; zero expected width accepts any type.
static inline xrif_error_t xrif_validate_raw( xrif_t handle, xrif_geometry *geometry, size_t element_bytes )
{
    xrif_error_t rv = xrif_geometry_get( handle, geometry );
    if( rv != XRIF_NOERROR )
    {
        return rv;
    }
    if( element_bytes != 0 && geometry->m_element_bytes != element_bytes )
    {
        return XRIF_ERROR_INVALID_TYPE;
    }
    if( handle->m_raw_buffer == NULL )
    {
        return XRIF_ERROR_NOT_SETUP;
    }
    return handle->m_raw_buffer_size < geometry->m_raw_bytes ? XRIF_ERROR_INSUFFICIENT_SIZE : XRIF_NOERROR;
}

/// Validate both transform buffers, including method padding, before forming pointers.
static inline xrif_error_t
xrif_validate_reorder( xrif_t handle, xrif_geometry *geometry, size_t element_bytes, int method )
{
    xrif_error_t rv = xrif_validate_raw( handle, geometry, element_bytes );
    if( rv != XRIF_NOERROR )
    {
        return rv;
    }
    size_t bytes = xrif_reordered_bytes( geometry, handle->m_frames, method );
    if( bytes == 0 )
    {
        return XRIF_ERROR_INVALID_SIZE;
    }
    if( handle->m_reordered_buffer == NULL )
    {
        return XRIF_ERROR_NOT_SETUP;
    }
    return handle->m_reordered_buffer_size < bytes ? XRIF_ERROR_INSUFFICIENT_SIZE : XRIF_NOERROR;
}

/// Validate source/destination storage for a codec using an already checked method bound.
static inline xrif_error_t xrif_validate_codec( xrif_t handle, size_t reordered_bytes, size_t bound, int decoding )
{
    if( handle == NULL )
    {
        return XRIF_ERROR_NULLPTR;
    }
    if( reordered_bytes == 0 || ( !decoding && bound == 0 ) )
    {
        return XRIF_ERROR_INVALID_SIZE;
    }
    char  *buffer   = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    size_t capacity = handle->m_compress_on_raw ? handle->m_raw_buffer_size : handle->m_compressed_buffer_size;
    if( buffer == NULL || handle->m_reordered_buffer == NULL )
    {
        return XRIF_ERROR_NOT_SETUP;
    }
    if( handle->m_reordered_buffer_size < reordered_bytes ||
        capacity < ( decoding ? handle->m_compressed_size : bound ) )
    {
        return XRIF_ERROR_INSUFFICIENT_SIZE;
    }
    if( decoding && handle->m_compressed_size == 0 )
    {
        return XRIF_ERROR_INVALID_SIZE;
    }
    return XRIF_NOERROR;
}

#endif
