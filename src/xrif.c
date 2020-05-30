/** \file xrif.c
  * \brief The eXtreme-ao Reordered Image Format: Definitions
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_files
  */

/* This file is part of the xrif library.

Copyright (c) 2019, 2020, The Arizona Board of Regents on behalf of The
University of Arizona

All rights reserved.

Developed by: The Arizona Board of Regents on behalf of the
University of Arizona.

Redistribution and use for noncommercial purposes in source and
binary forms, with or without modification, are permitted provided
that the following conditions are met:

1. The software is used solely for noncommercial purposes.

2. Redistributions of source code must retain the above copyright
notice, terms and conditions specified herein and the disclaimer
specified in Section 4 below.

3. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

4. Neither the name of the Arizona Board of Regents, the University
of Arizona nor the names of other contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

Arizona Required Clauses:

1.1. Arbitration. The parties agree that if a dispute arises
between them concerning this Agreement, the parties may be required
to submit the matter to arbitration pursuant to Arizona law.

1.2. Applicable Law and Venue. This Agreement shall be interpreted
pursuant to the laws of the State of Arizona. Any arbitration or
litigation between the Parties shall be conducted in Pima County,
ARIZONA, and LICENSEE hereby submits to venue and jurisdiction in
Pima County, ARIZONA.

1.3. Non-Discrimination. The Parties agree to be bound by state and
federal laws and regulations governing equal opportunity and non-
discrimination and immigration.

1.4. Appropriation of Funds. The Parties recognize that performance
by ARIZONA may depend upon appropriation of funds by the State
Legislature of ARIZONA. If the Legislature fails to appropriate the
necessary funds, or if ARIZONA’S appropriation is reduced during
the fiscal year, ARIZONA may cancel this Agreement without further
duty or obligation. ARIZONA will notify LICENSEE as soon as
reasonably possible after it knows of the loss of funds.

1.5. Conflict of Interest. This Agreement is subject to the
provisions of A.R.S. 38-511 and other conflict of interest
regulations. Within three years of the EFFECTIVE DATE, ARIZONA may
cancel this Agreement if any person significantly involved in
initiating, negotiating, drafting, securing, or creating this
Agreement for or on behalf of ARIZONA becomes an employee or
consultant in any capacity of LICENSEE with respect to the subject
matter of this Agreement.

*/

#include "xrif.h"

#define BIT15 (32768)
#define BIT14 (16384)
#define BIT13 (8192)
#define BIT12 (4096)
#define BIT11 (2048)
#define BIT10 (1024)
#define BIT09 (512)
#define BIT08 (256)
#define BIT07 (128)
#define BIT06 (64)
#define BIT05 (32)
#define BIT04 (16)
#define BIT03 (8)
#define BIT02 (4)
#define BIT01 (2)
#define BIT00 (1)

// Allocate an xrif_handle object, which is pointed to by an xrif_t
xrif_error_t xrif_new(xrif_t * handle_ptr)
{
   if( handle_ptr == NULL) return XRIF_ERROR_NULLPTR;
   
   *handle_ptr = (xrif_t) malloc( sizeof(xrif_handle) );
   
   if( *handle_ptr == NULL) return XRIF_ERROR_MALLOC;
   
   return xrif_initialize_handle(*handle_ptr);
   
}

// De-allocate an xrif_handle object, which is pointed to by an xrif_t
xrif_error_t xrif_delete(xrif_t handle )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   xrif_error_t rv = xrif_reset_handle( handle );
   
   free(handle);
   
   return rv;
}

// Initialize an xrif handle object.
xrif_error_t xrif_initialize_handle( xrif_t handle )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   handle->width = 0;
   handle->height = 0;
   handle->depth = 0;
   handle->frames = 0;
   
   handle->type_code = 0;
   
   handle->data_size = 0;

   handle->raw_size = 0;
   handle->compressed_size = 0;
   
   handle->difference_method = XRIF_DIFFERENCE_DEFAULT;
   
   handle->reorder_method = XRIF_REORDER_DEFAULT;
   
   handle->compress_method = XRIF_COMPRESS_DEFAULT;
   
   handle->lz4_acceleration = 1;

   handle->omp_parallel = 0;
   handle->omp_numthreads = 1;
   
   handle->compress_on_raw = 1;
   
   handle->own_raw = 0;
   handle->raw_buffer = 0;
   handle->raw_buffer_size = 0;
   
   handle->own_reordered = 0;
   handle->reordered_buffer = 0;
   handle->reordered_buffer_size = 0;
   
   handle->own_compressed = 0;
   handle->compressed_buffer = 0;
   handle->compressed_buffer_size = 0;
   
   handle->calc_performance = 1; 
   
   handle->compression_ratio = 0;
   handle->encode_time = 0;
   handle->encode_rate = 0;
   handle->difference_time = 0;
   handle->difference_rate = 0;
   handle->reorder_time = 0;
   handle->reorder_rate = 0;
   handle->compress_time = 0;
   handle->compress_rate = 0;
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_set_size( xrif_t handle,
                            xrif_dimension_t w,
                            xrif_dimension_t h,
                            xrif_dimension_t d,
                            xrif_dimension_t f,
                            xrif_typecode_t c
                          )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if(w == 0 || h == 0 || d == 0 || f == 0) return XRIF_ERROR_INVALID_SIZE;
   
   size_t sz = xrif_typesize(c);
   
   if(sz == 0) return XRIF_ERROR_INVALID_TYPE;
   
   handle->width = w;
   handle->height = h;
   handle->depth = d;
   handle->frames = f;
   handle->type_code = c;
   
   handle->data_size = sz;
   
   handle->raw_size = w*h*d*f*handle->data_size;
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_set_difference_method( xrif_t handle,
                                         int difference_method
                                       )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if( difference_method == XRIF_DIFFERENCE_NONE ) handle->difference_method = XRIF_DIFFERENCE_NONE;
   else if( difference_method == XRIF_DIFFERENCE_DEFAULT ) handle->difference_method = XRIF_DIFFERENCE_DEFAULT;
   else if( difference_method == XRIF_DIFFERENCE_PREVIOUS ) handle->difference_method = XRIF_DIFFERENCE_PREVIOUS;
   else if( difference_method == XRIF_DIFFERENCE_FIRST ) handle->difference_method = XRIF_DIFFERENCE_FIRST;
   else if( difference_method == XRIF_DIFFERENCE_PIXEL ) handle->difference_method = XRIF_DIFFERENCE_PIXEL;
   else
   {
      difference_method == XRIF_DIFFERENCE_DEFAULT;
      return XRIF_ERROR_BADARG;
   }
   
   return XRIF_NOERROR;
}


xrif_error_t xrif_set_reorder_method( xrif_t handle,
                                      int reorder_method
                                    )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if( reorder_method == XRIF_REORDER_NONE ) handle->reorder_method = XRIF_REORDER_NONE;
   else if( reorder_method == XRIF_REORDER_DEFAULT ) handle->reorder_method = XRIF_REORDER_DEFAULT;
   else if( reorder_method == XRIF_REORDER_BYTEPACK ) handle->reorder_method = XRIF_REORDER_BYTEPACK;
   else if( reorder_method == XRIF_REORDER_BYTEPACK_RENIBBLE ) handle->reorder_method = XRIF_REORDER_BYTEPACK_RENIBBLE;
   else if( reorder_method == XRIF_REORDER_BITPACK ) handle->reorder_method = XRIF_REORDER_BITPACK;
   else
   {
      reorder_method == XRIF_REORDER_DEFAULT;
      return XRIF_ERROR_BADARG;
   }
   
   return XRIF_NOERROR;
}

// Set the compress method.
xrif_error_t xrif_set_compress_method( xrif_t handle,
                                       int compress_method
                                     )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if( compress_method == XRIF_COMPRESS_NONE ) handle->compress_method = XRIF_COMPRESS_NONE;
   else if( compress_method == XRIF_COMPRESS_DEFAULT ) handle->compress_method = XRIF_COMPRESS_DEFAULT;
   else if( compress_method == XRIF_COMPRESS_LZ4 ) handle->compress_method = XRIF_COMPRESS_LZ4;
   else
   {
      compress_method == XRIF_COMPRESS_DEFAULT;
      return XRIF_ERROR_BADARG;
   }
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_set_raw( xrif_t handle,
                           void * raw,          
                           size_t size          
                         )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if(handle->raw_buffer && handle->own_raw )
   {
      free(handle->raw_buffer);
   }

   handle->own_raw = 0;

   handle->raw_buffer = raw;
   
   handle->raw_buffer_size = size;
   
   if((handle->raw_buffer != NULL && handle->raw_buffer_size == 0) || (handle->raw_buffer == 0 && handle->raw_buffer_size != 0)) 
   {
      return XRIF_ERROR_INVALID_SIZE;
   }
   
   //Check if we meet minimum size requirement based on compress_on_raw value
   size_t minSz;
   
   if(handle->compress_on_raw)
   {
      //Set size to be the larger of LZ4_compressBound and the data size:
   
      minSz = LZ4_compressBound(handle->width * handle->height * handle->depth * handle->frames * handle->data_size);
   
      if(minSz < handle->width * handle->height * handle->depth * handle->frames * handle->data_size) 
      {
         minSz = handle->width * handle->height * handle->depth * handle->frames * handle->data_size;
      }
   }
   else
   {
      //Just set to the minimum size
      minSz = handle->width * handle->height * handle->depth * handle->frames * handle->data_size;
   }
      
   if(handle->raw_buffer_size < minSz) return XRIF_ERROR_INSUFFICIENT_SIZE;
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_allocate_raw( xrif_t handle )
{
   if(handle->raw_buffer && handle->own_raw )
   {
      free(handle->raw_buffer);
   }
   
   if(handle->width * handle->height * handle->depth * handle->frames * handle->data_size == 0) return XRIF_ERROR_NOT_SETUP;
   
   if(handle->compress_on_raw)
   {
      //Set size to be the larger of LZ4_compressBound and the data size:
   
      handle->raw_buffer_size = LZ4_compressBound(handle->width * handle->height * handle->depth * handle->frames * handle->data_size);
   
      if(handle->raw_buffer_size < handle->width * handle->height * handle->depth * handle->frames * handle->data_size) 
      {
         handle->raw_buffer_size= handle->width * handle->height * handle->depth * handle->frames * handle->data_size;
      }
   }
   else
   {
      //Just set to the minimum size
      handle->raw_buffer_size = handle->width * handle->height * handle->depth * handle->frames * handle->data_size;
   }
   
   handle->raw_buffer = (char *) malloc( handle->raw_buffer_size );
   
   if(handle->raw_buffer == NULL) 
   {
      handle->own_raw = 0;
      return XRIF_ERROR_MALLOC;
   }
   
   handle->own_raw = 1;
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_set_reordered( xrif_t handle,
                                 void * reordered,
                                 size_t size
                               )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if(handle->reordered_buffer && handle->own_reordered )
   {
      free(handle->reordered_buffer);
   }
   
   handle->reordered_buffer = reordered;
   
   handle->reordered_buffer_size = size;
   
   handle->own_reordered = 0;
   
   
   if((handle->reordered_buffer != NULL && handle->reordered_buffer_size == 0) || (handle->reordered_buffer == 0 && handle->reordered_buffer_size != 0))
   {
      return XRIF_ERROR_INVALID_SIZE;
   }
   
   if(handle->reordered_buffer_size < handle->width * handle->height * handle->depth * handle->frames * handle->data_size) return XRIF_ERROR_INSUFFICIENT_SIZE;
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_allocate_reordered( xrif_t handle )
{
   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if(handle->reordered_buffer && handle->own_reordered)
   {
      free(handle->reordered_buffer);
   }

   // *********** Make reordered buffer big enough ************************
   size_t one_frame = handle->width*handle->height*handle->depth*handle->data_size;

   if(one_frame < 16) one_frame = 16; //Handle cases where too little data is available for bitpacking.
   
   handle->reordered_buffer_size = one_frame * (handle->frames+1); //Allocating an extra frame to allow for odd number of pixels in reorder step
   
   //printf("mod16: %ld\n", (handle->reordered_buffer_size - one_frame) % 16);
   
   handle->reordered_buffer = (char *) malloc( handle->reordered_buffer_size );
   
   if(handle->reordered_buffer == NULL) 
   {
      handle->reordered_buffer_size = 0;
      handle->own_reordered = 0;
      return XRIF_ERROR_MALLOC;
   }
   
   handle->own_reordered = 1;
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_set_compressed( xrif_t handle, 
                                  void * compressed,    
                                  size_t size           
                                )
{

   if( handle == NULL) return XRIF_ERROR_NULLPTR;
   
   if(handle->compressed_buffer && handle->own_compressed )
   {
      free(handle->compressed_buffer);
   }

   handle->own_compressed = 0;

   handle->compressed_buffer = compressed;
   
   handle->compressed_buffer_size = size;
   
   if((handle->compressed_buffer != NULL && handle->compressed_buffer_size == 0) || (handle->compressed_buffer == 0 && handle->compressed_buffer_size != 0)) return XRIF_ERROR_INVALID_SIZE;
   
   if(handle->compressed_buffer_size < LZ4_compressBound(handle->width * handle->height * handle->depth * handle->frames * handle->data_size)) return XRIF_ERROR_INSUFFICIENT_SIZE;
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_allocate_compressed( xrif_t handle )
{
   if(handle->compressed_buffer && handle->own_compressed )
   {
      free(handle->compressed_buffer);
   }
   
   if(handle->width * handle->height * handle->depth * handle->frames * handle->data_size == 0) return XRIF_ERROR_NOT_SETUP;
   
   handle->compressed_buffer_size = LZ4_compressBound(handle->width * handle->height * handle->depth * handle->frames * handle->data_size);
   
   handle->compressed_buffer = (char *) malloc( handle->compressed_buffer_size );
   
   if(handle->compressed_buffer == NULL) 
   {
      handle->own_compressed = 0;
      return XRIF_ERROR_MALLOC;
   }
   
   handle->own_compressed = 1;
   
   return XRIF_NOERROR;
}


xrif_error_t xrif_allocate( xrif_t handle )
{
   xrif_error_t rv;
   
   rv = xrif_allocate_raw(handle);
   if(rv < 0) return rv;
   
   rv = xrif_allocate_reordered(handle);
   if(rv < 0) return rv;
   
   if(handle->compress_on_raw == 0)
   {
      rv = xrif_allocate_compressed(handle);
      if(rv < 0) return rv;
   }
   
   return XRIF_NOERROR;

}

// Reset a handle, restoring it to the initialized state. De-allocates owned pointers and re-initializes.
xrif_error_t xrif_reset_handle( xrif_t handle )
{
   if(handle->reordered_buffer && handle->own_reordered)
   {
      free(handle->reordered_buffer);
   }
   
   if(handle->raw_buffer && handle->own_raw )
   {
      free(handle->raw_buffer);
   }
   
   if(handle->compressed_buffer && handle->own_compressed)
   {
      free(handle->compressed_buffer);
   }
   
   return xrif_initialize_handle(handle);
   
}

xrif_error_t xrif_write_header( char * header,
                                xrif_t handle 
                              )
{
   header[0] = 'x';
   header[1] = 'r';
   header[2] = 'i';
   header[3] = 'f';
   
   *((uint32_t *) &header[4]) = XRIF_VERSION;
   
   *((uint32_t *) &header[8]) = XRIF_HEADER_SIZE; 

   *((uint32_t *) &header[12]) = handle->width;
   
   *((uint32_t *) &header[16]) = handle->height;
   
   *((uint32_t *) &header[20]) = handle->depth;
   
   *((uint32_t *) &header[24]) = handle->frames;
   
   *((uint16_t *) &header[28]) = handle->type_code;

   *((int16_t *) &header[30]) = handle->difference_method;
   
   *((int16_t *) &header[32]) = handle->reorder_method;

   *((int16_t *) &header[34]) = handle->compress_method;

   *((uint32_t *) &header[36]) = handle->compressed_size;
   
   memset(&header[40], 0, 8);
   
   if(handle->compress_method == XRIF_COMPRESS_LZ4)
   {
      *((uint16_t *) &header[40]) = handle->lz4_acceleration;
   }
   
   return XRIF_NOERROR;
   
}


xrif_error_t xrif_read_header( xrif_t handle,
                               uint32_t * header_size,
                               char * header
                             )
{
   if( header[0] != 'x' || header[1] != 'r' || header[2] != 'i' || header[3] != 'f')
   {
      return XRIF_ERROR_BADHEADER;
   }
   
   if( *((uint32_t *) &header[4]) != XRIF_VERSION)
   {
      return XRIF_ERROR_WRONGVERSION;
   }
   
   *header_size = *((uint32_t *) &header[8]); 

   handle->width = *((uint32_t *) &header[12]);
   
   handle->height = *((uint32_t *) &header[16]);
   
   handle->depth = *((uint32_t *) &header[20]);
   
   handle->frames = *((uint32_t *) &header[24]);
   
   handle->type_code = *((uint16_t *) &header[28]);

   handle->data_size = xrif_typesize(handle->type_code);
   
   handle->difference_method = *((int16_t *) &header[30]);
   
   handle->reorder_method = *((int16_t *) &header[32]);

   handle->compress_method = *((int16_t *) &header[34]);

   handle->compressed_size = *((uint32_t *) &header[36]);
   
   if(handle->compress_method == XRIF_COMPRESS_LZ4)
   {
      handle->lz4_acceleration = *((uint16_t *) &header[40]);
   }
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_encode( xrif_t handle )
{
   xrif_error_t rv; 
   
   clock_gettime(CLOCK_REALTIME, &handle->ts_difference_start);
   
   rv = xrif_difference( handle);
   
   if( rv != XRIF_NOERROR ) return rv;
   
   clock_gettime(CLOCK_REALTIME, &handle->ts_reorder_start);
   
   rv = xrif_reorder(handle);
   
   if( rv != XRIF_NOERROR ) return rv;
   
   clock_gettime(CLOCK_REALTIME, &handle->ts_compress_start);

   rv = xrif_compress(handle);
   
   if( rv != XRIF_NOERROR ) return rv;
   
   clock_gettime(CLOCK_REALTIME, &handle->ts_compress_done);

   
   if(handle->calc_performance)
   {
      handle->compression_ratio = xrif_compression_ratio(handle);
      handle->encode_time = xrif_encode_time(handle);
      handle->encode_rate = xrif_encode_rate(handle);
      handle->difference_time = xrif_difference_time(handle);
      handle->difference_rate = xrif_difference_rate(handle);
      handle->reorder_time = xrif_reorder_time(handle);
      handle->reorder_rate = xrif_reorder_rate(handle);
      handle->compress_time = xrif_compress_time(handle);
      handle->compress_rate = xrif_compress_rate(handle);
   }
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_decode( xrif_t handle )
{
   xrif_error_t rv;
   
   clock_gettime(CLOCK_REALTIME, &handle->ts_decompress_start);
   
   rv = xrif_decompress(handle);
   
   if( rv != XRIF_NOERROR ) 
   {
      fprintf(stderr, "xrif_decode: error returned by xrif_decompress\n");
      return rv;
   }
   clock_gettime(CLOCK_REALTIME, &handle->ts_unreorder_start);
   
   rv = xrif_unreorder(handle);
   
   if( rv != XRIF_NOERROR )
   {
      fprintf(stderr, "xrif_decode: error returned by xrif_unreorder\n");
      return rv;
   }

    
   clock_gettime(CLOCK_REALTIME, &handle->ts_undifference_start);
      
   rv = xrif_undifference(handle);

   if( rv != XRIF_NOERROR )
   {
      fprintf(stderr, "xrif_decode: error returned by xrif_undifference\n");
      return rv;
   }

    
   clock_gettime(CLOCK_REALTIME, &handle->ts_undifference_done);
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_difference( xrif_t handle )
{
   int method = handle->difference_method;
   
   if(method == 0) method = XRIF_DIFFERENCE_DEFAULT;
   
   switch( method )
   {
      case XRIF_DIFFERENCE_NONE:
         return XRIF_NOERROR;
      case XRIF_DIFFERENCE_PREVIOUS:
         return xrif_difference_previous_sint16(handle);
      case XRIF_DIFFERENCE_FIRST:
         return xrif_difference_first_sint16(handle);
      case XRIF_DIFFERENCE_PIXEL:
         return xrif_difference_pixel_sint16(handle);
      default:
         return XRIF_ERROR_NOTIMPL;
   }
}

xrif_error_t xrif_undifference( xrif_t handle )
{
   int method = handle->difference_method;
   
   if(method == 0) method = XRIF_DIFFERENCE_DEFAULT;
   
   switch( method )
   {
      case XRIF_DIFFERENCE_NONE:
         return XRIF_NOERROR;
      case XRIF_DIFFERENCE_PREVIOUS:
         return xrif_undifference_previous_sint16(handle);
      case XRIF_DIFFERENCE_FIRST:
         return XRIF_ERROR_NOTIMPL;
      case XRIF_DIFFERENCE_PIXEL:
         return xrif_difference_pixel_sint16(handle);
      default:
         return XRIF_ERROR_NOTIMPL;
   }
}

xrif_error_t xrif_difference_previous_sint16( xrif_t handle )
{
   for(int n=0; n < handle->frames-1; ++n)
   {
      size_t n_stride0 = (handle->frames - 2 - n) * handle->height*handle->width*handle->depth;
      size_t n_stride =  (handle->frames - 1 - n) * handle->height*handle->width*handle->depth;

      for(int kk=0; kk< handle->depth; ++kk)
      {
         size_t kk_stride = kk*handle->height*handle->width;
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
         
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0 + kk_stride + ii_stride  + jj;
               size_t idx = n_stride + kk_stride + ii_stride  + jj;
            
               //int F = ((int16_t *)handle->raw_buffer)[idx];
               //int L = ((int16_t*)handle->raw_buffer)[idx0];
               //unsigned int16_t D = abs(F-L);
               ((int16_t *) handle->raw_buffer)[idx] = (((int16_t *)handle->raw_buffer)[idx] - ((int16_t*)handle->raw_buffer)[idx0]);
            }
         }
         
      }
   } 
   
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_difference_previous_uint64( xrif_t handle )
{
   for(int n=0; n < handle->frames-1; ++n)
   {
      size_t n_stride0 = (handle->frames - 2 - n) * handle->height*handle->width*handle->depth;
      size_t n_stride =  (handle->frames - 1 - n) * handle->height*handle->width*handle->depth;

      for(int kk=0; kk< handle->depth; ++kk)
      {
         size_t kk_stride = kk*handle->height*handle->width;
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
         
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0 + kk_stride + ii_stride  + jj;
               size_t idx = n_stride + kk_stride + ii_stride  + jj;
            
               ((int64_t *) handle->raw_buffer)[idx] = (((uint64_t *)handle->raw_buffer)[idx] - ((uint64_t*)handle->raw_buffer)[idx0]);
            }
         }
         
      }
   } 
   
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_difference_first_sint16( xrif_t handle )
{
   for(int n=0; n < handle->frames-1; ++n)
   {
      size_t n_stride0 = 0;// * handle->height*handle->width*handle->depth;
      size_t n_stride =  (handle->frames - 1 - n) * handle->height*handle->width*handle->depth;

      for(int kk=0; kk< handle->depth; ++kk)
      {
         size_t kk_stride = kk*handle->height*handle->width;
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
         
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0 + kk_stride + ii_stride  + jj;
               size_t idx = n_stride + kk_stride + ii_stride  + jj;
            
               ((int16_t *) handle->raw_buffer)[idx] = ((int16_t *)handle->raw_buffer)[idx] - ((int16_t*)handle->raw_buffer)[idx0];
            }
         }
      }
   }   
   return XRIF_NOERROR;
}

xrif_error_t xrif_difference_first_uint64( xrif_t handle )
{
   for(int n=0; n < handle->frames-1; ++n)
   {
      size_t n_stride0 = 0;// * handle->height*handle->width*handle->depth;
      size_t n_stride =  (handle->frames - 1 - n) * handle->height*handle->width*handle->depth;

      for(int kk=0; kk< handle->depth; ++kk)
      {
         size_t kk_stride = kk*handle->height*handle->width;
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
         
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0 + kk_stride + ii_stride  + jj;
               size_t idx = n_stride + kk_stride + ii_stride  + jj;
            
               ((int64_t *) handle->raw_buffer)[idx] = ((uint64_t *)handle->raw_buffer)[idx] - ((uint64_t*)handle->raw_buffer)[idx0];
            }
         }
      }
   }   
   return XRIF_NOERROR;
}

xrif_error_t xrif_difference_pixel_sint16( xrif_t handle )
{
   for(int n=0; n < handle->frames-1; ++n)
   {
      for(int kk=0; kk< handle->depth; ++kk)
      {
         size_t idx0 = kk*handle->height*handle->width;
         int16_t pix0 = ((int16_t*)handle->raw_buffer)[idx0];  //To avoid an extra if, we are going to overwrite this
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx = idx0 + ii_stride  + jj;
            
               ((int16_t *) handle->raw_buffer)[idx] = ((int16_t *) handle->raw_buffer)[idx] - pix0;
            }
         }
         ((int16_t*)handle->raw_buffer)[idx0] = pix0; //Now restore the first pixel.
      }
   }   
   return XRIF_NOERROR;
}

xrif_error_t xrif_difference_sint16_rgb( xrif_t handle )
{
   for(int n=0; n < handle->frames-1; ++n)
   {
      if( (handle->frames - 1 - n) % 3 == 0)
      {
         size_t n_stride0 = (handle->frames - 4 - n) * handle->height*handle->width;
         size_t n_stride =  (handle->frames - 1 - n) * handle->height*handle->width;
      
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
        
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0  + ii_stride  + jj;
               size_t idx = n_stride + ii_stride  + jj;
            
               ((int16_t *) handle->raw_buffer)[idx] = (((int16_t *)handle->raw_buffer)[idx] - ((int16_t*)handle->raw_buffer)[idx0]);
            }
         }
      }
      else
      {
         size_t n_stride0 = (handle->frames - 2 - n) * handle->height*handle->width;
         size_t n_stride =  (handle->frames - 1 - n) * handle->height*handle->width;
      
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
        
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0  + ii_stride  + jj;
               size_t idx = n_stride + ii_stride  + jj;
            
               ((int16_t *) handle->raw_buffer)[idx] = (((int16_t *)handle->raw_buffer)[idx] - ((int16_t*)handle->raw_buffer)[idx0]);
            }
         }
      }
   }
   
   for(int n=0; n < handle->frames-3; ++n)
   {
      if( (handle->frames - 1 - n) % 3 == 0) continue;
      else
      {
         size_t n_stride0 = (handle->frames - 4 - n) * handle->height*handle->width;
         size_t n_stride =  (handle->frames - 1 - n) * handle->height*handle->width;
      
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
        
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0  + ii_stride  + jj;
               size_t idx = n_stride + ii_stride  + jj;
            
               ((int16_t *) handle->raw_buffer)[idx] = (((int16_t *)handle->raw_buffer)[idx] - ((int16_t*)handle->raw_buffer)[idx0]);
            }
         }
      }
   }
   return XRIF_NOERROR;
}

xrif_error_t xrif_undifference_previous_sint16( xrif_t handle )
{
   for(int n=1; n < handle->frames; ++n)
   {
      size_t n_stride0 = (n-1) * handle->height*handle->width*handle->depth;
      size_t n_stride =  n * handle->height*handle->width*handle->depth;
      
      for(int kk=0; kk<handle->depth; ++kk)
      {
         size_t kk_stride = kk*handle->height*handle->width;
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
            
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0 + kk_stride + ii_stride  + jj;
               size_t idx = n_stride + kk_stride + ii_stride  + jj;
               
               ((int16_t *) handle->raw_buffer)[idx] = ((int16_t *)handle->raw_buffer)[idx] + ((int16_t*)handle->raw_buffer)[idx0];
            }
         }
      }
   }
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_undifference_previous_uint64( xrif_t handle )
{
   for(int n=1; n < handle->frames; ++n)
   {
      size_t n_stride0 = (n-1) * handle->height*handle->width*handle->depth;
      size_t n_stride =  n * handle->height*handle->width*handle->depth;
      
      for(int kk=0; kk<handle->depth; ++kk)
      {
         size_t kk_stride = kk*handle->height*handle->width;
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
            
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx0 =  n_stride0 + kk_stride + ii_stride  + jj;
               size_t idx = n_stride + kk_stride + ii_stride  + jj;
               
               ((uint64_t *) handle->raw_buffer)[idx] = ((int64_t *)handle->raw_buffer)[idx] + ((int64_t *)handle->raw_buffer)[idx0];
            }
         }
      }
   }
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_undifference_pixel_sint16( xrif_t handle )
{
   for(int n=0; n < handle->frames-1; ++n)
   {
      for(int kk=0; kk< handle->depth; ++kk)
      {
         size_t idx0 = kk*handle->height*handle->width;
         int16_t pix0 = ((int16_t*)handle->raw_buffer)[idx0]; //To avoid an extra if, we are going to overwrite this
         
         for(int ii=0; ii< handle->width; ++ii)
         {
            size_t ii_stride = ii*handle->height;
            for(int jj=0; jj< handle->height; ++jj)
            {
               size_t idx = idx0 + ii_stride  + jj;
            
               ((int16_t *) handle->raw_buffer)[idx] = ((int16_t *) handle->raw_buffer)[idx] + pix0;
            }
         }
         ((int16_t*)handle->raw_buffer)[idx0] = pix0; //Now restore the first pixel.
      }
   }   
   return XRIF_NOERROR;
}

xrif_error_t xrif_reorder( xrif_t handle )
{
   int method = handle->reorder_method;
   
   if(method == 0) method = XRIF_REORDER_DEFAULT;
   
   switch( method )
   {
      case XRIF_REORDER_NONE:
         return xrif_reorder_none(handle);
      case XRIF_REORDER_BYTEPACK:
         return xrif_reorder_bytepack(handle);
      case XRIF_REORDER_BYTEPACK_RENIBBLE:
         return xrif_reorder_bytepack_renibble(handle);
      case XRIF_REORDER_BITPACK:
         return xrif_reorder_bitpack(handle);
      default:
         return XRIF_ERROR_NOTIMPL;
   }
}

xrif_error_t xrif_unreorder( xrif_t handle )
{
   int method = handle->reorder_method;
   
   if(method == 0) method = XRIF_REORDER_DEFAULT;
   
   switch( method )
   {
      case XRIF_REORDER_NONE:
         return xrif_unreorder_none(handle);
      case XRIF_REORDER_BYTEPACK:
         return xrif_unreorder_bytepack(handle);
      case XRIF_REORDER_BYTEPACK_RENIBBLE:
         return xrif_unreorder_bytepack_renibble(handle);
      case XRIF_REORDER_BITPACK:
         return xrif_unreorder_bitpack(handle); 
      default:
         return XRIF_ERROR_NOTIMPL;
   }
}


xrif_error_t xrif_reorder_none( xrif_t handle )
{
   size_t npix = handle->width * handle->height * handle->depth * handle->frames; 
   
   if( handle->raw_buffer_size < npix*handle->data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   if( handle->reordered_buffer_size < npix*handle->data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   //Zero the reordered buffer.
   memset(handle->reordered_buffer, 0, handle->reordered_buffer_size); ///\todo this can be just the extra pixels
   
   memcpy(handle->reordered_buffer, handle->raw_buffer, npix*handle->data_size);
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_reorder_bytepack( xrif_t handle )
{
   size_t one_frame = handle->width*handle->height* handle->depth *handle->data_size; //bytes
   
   size_t npix = handle->width * handle->height * handle->depth * (handle->frames-1); //pixels not bytes
   
   char * raw_buffer = handle->raw_buffer + one_frame ;
   char * reordered_buffer = handle->reordered_buffer + one_frame;
   char * reordered_buffer2 = reordered_buffer + npix;

   if( handle->raw_buffer_size < one_frame + npix*handle->data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   if( handle->reordered_buffer_size < one_frame + npix*handle->data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   //Zero the reordered buffer.
   memset(handle->reordered_buffer, 0, handle->reordered_buffer_size); ///\todo this can be just the extra pixels
   
   //#pragma omp parallel for
   for(size_t pix=0; pix<one_frame; ++pix)
   {
      handle->reordered_buffer[pix] = handle->raw_buffer[pix];
   }
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      //Note: a lookup table for this was found to be 2x slower than the following algorithm.
      int_fast8_t x2 = raw_buffer[2*pix];
      int_fast8_t x1 = raw_buffer[2*pix+1];

      if(x2 < 0)
      {
         if(x1 == -1)  x1 = 0;
         else if(x1 == 0) x1 = -1;
      }
         
       reordered_buffer[pix] = x2; 
       reordered_buffer2[pix] = x1;
      
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}

xrif_error_t xrif_reorder_bytepack_renibble( xrif_t handle )
{
   //The lookup table, a static array
   #include "bitshift_and_nibbles.inc"
   
   size_t one_frame = handle->width*handle->height* handle->depth *handle->data_size; //bytes
   
   size_t npix = handle->width * handle->height * handle->depth * (handle->frames-1); //pixels not bytes
   
   if( handle->raw_buffer_size < one_frame + npix*handle->data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   //The reordered buffer must be frames+1 big to be sure it can handle odd sizes. 
   ///\todo need a function to calculate required size
   if( handle->reordered_buffer_size < handle->width * handle->height * handle->depth * (handle->frames+1)*handle->data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   int16_t * raw_buffer = (int16_t*)(handle->raw_buffer + one_frame);
   
   //Get pointer that starts one image into the handle->reordered_buffer.  This area is 2*npix bytes long
   unsigned char * reordered_buffer = (unsigned char *) handle->reordered_buffer + one_frame;
   
   //Get point that starts halfway through, splitting at npix bytes
   unsigned char * reordered_buffer2 = (unsigned char*) reordered_buffer + npix; 
   
   //Zero the reordered buffer.
   memset(handle->reordered_buffer, 0, handle->reordered_buffer_size); ///\todo this can be just the extra pixels
      
   //Set the first part of the reordered buffer to the first frame (always the reference frame)
   for(size_t pix=0; pix< one_frame; ++pix)
   {
      handle->reordered_buffer[pix] = handle->raw_buffer[pix];
   }
   
   //Corrections necessary to handle odd numbers
   size_t halfoff = ((double) npix)/2.0 + 0.5;
   size_t oneoff = 0;
   if(halfoff > npix/2) oneoff = 0;
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      /* This block of commented code is left in to document the algorithm implemented in the lookup table.
       * And maybe we'll implement some defines to avoid lookup tables . . .
       */
      /*
      int_fast16_t s = raw_buffer[pix]; //Get the first 2 bytes
      int_fast16_t sbit = (s < 0); //and the signbit

      s *= (1-2*sbit); //make it positive
      
      uint16_t us = ( (s) << 1) | sbit; //This moves the sign bit
       
      reordered_buffer[pix] = ((char *)(&us))[0]; //store first byte, which includes the sign bit?

      // Note: A pre-calculated table look-up for just nibble values produced slightly slower code.
      uint16_t nib1, nib2;
      
      if(pix % 2 == 0)
      {
         nib1 = ((unsigned char *)(&us))[1] << 4; //Move low 4 to high 4
         nib2 = ((unsigned char *)(&us))[1] & 240; //Select the high 4
      }
      else
      {
         nib1 = (((unsigned char *)(&us))[1] & 15); //Select the low 4
         nib2 = ((unsigned char *)(&us))[1] >> 4; //Move the high 4 to the low 4
      }
      
      reordered_buffer2[pix/2] |= nib1;
      reordered_buffer2[pix/2 + oneoff + halfoff] |= nib2;
         
      /**/
      //Here we use a lookup table calculated according to the above algorithm:

      const unsigned char * bsn = &bitshift_and_nibbles[ ((uint16_t) raw_buffer[pix]) * 6 + (pix&1)*3];
      reordered_buffer[pix] = (char) bsn[0];
      reordered_buffer2[pix/2] += bsn[1];
      reordered_buffer2[pix/2 + oneoff + halfoff] += bsn[2];
      /**/
   }
   #ifndef XRIF_NO_OMP
   }
   #endif
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_reorder_bitpack( xrif_t handle )
{
   //The lookup tables (static arrays)
   #include "bit_to_position.inc"
   #include "set_bits.inc"
   
   size_t one_frame = handle->width*handle->height* handle->depth *handle->data_size;
   
   for(size_t pix=0; pix < one_frame; ++pix)
   {
      handle->reordered_buffer[pix] = handle->raw_buffer[pix];
   }
   
   int16_t * raw_buffer = (int16_t *) (handle->raw_buffer + one_frame);
   uint16_t * reordered_buffer = (uint16_t *) (handle->reordered_buffer + one_frame);
   
   size_t npix = handle->width * handle->height * handle->depth * (handle->frames-1); 

   memset( (char *) reordered_buffer, 0, handle->reordered_buffer_size - one_frame);

   size_t stride = (handle->reordered_buffer_size - one_frame)/16/2; //stride in 16-bit pixels, not bytes
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->omp_parallel > 0) 
   {
   #pragma omp for
   #endif
      
  
   for(size_t pix = 0; pix<npix; ++pix)
   {
      size_t sbyte = pix/16; //This is the starting byte for this pixel
      int_fast8_t bit = pix % 16; //This is the bit position for this pixel
     
      
      //--- Reordering sign bit [left in to document]
      //int16_t s = raw_buffer[pix];
      //int8_t sbit = (s < 0);
      //s *= (1-2*sbit); //make positive
      //uint16_t us = ( (s) << 1) | sbit; //shift by 1, putting sign bit in highest entropy spot
      
      //--- Simpler way to reorder, this is equivalent to above
      int8_t sbit = (raw_buffer[pix] < 0);
      uint16_t us = 2*raw_buffer[pix]*(1-2*sbit)+sbit;
     

      //---- This is the basic algorithm, without lookup tables:
      /*
      for(int b=0; b < 16; ++b)
      {
         reordered_buffer[sbyte + b*stride] += (((us >> b) & 1) << bit);
      }
      */
      
      //---- A full lookup table version
      //Attempt with lookup table is slower, leaving this in to document this, possibly for future defines:
      //uint16_t us = left_shift_one[*((uint16_t *) &raw_buffer[pix])];

      //---- A faster lookup table version:
      size_t sbyte8 = sbyte + 8*stride;
      size_t st0 = ((unsigned char *) &us)[0]*16*8 + bit*8;
      size_t st1 = ((unsigned char *) &us)[1]*16*8 + bit*8;
      
      for(int_fast8_t b = 0; b < 8; ++b)
      {  
         reordered_buffer[sbyte +  b*stride] += bit_to_position[st0 + b] ;
      }
      if(((unsigned char *) &us)[1] == 0) continue;
      
      //Get the number of set bits, and for this second byte we only process those bits.
      unsigned char nbits2 = set_bits[((unsigned char *) &us)[1]*9];
      const unsigned char* bits2 = &set_bits[((unsigned char *) &us)[1]*9 + 1];
      for(int_fast8_t b = 0; b < nbits2; ++b)
      {      
         reordered_buffer[sbyte8 +  bits2[b]*stride] +=  bit_to_position[st1 + bits2[b]];
      }
      
      
      
   }
   #ifndef XRIF_NO_OMP
   }
   #endif
   
   return XRIF_NOERROR;
}
      

         
xrif_error_t xrif_unreorder_none( xrif_t handle )
{
   size_t npix = handle->width * handle->height * handle->depth * handle->frames; 
   
   memcpy(handle->raw_buffer, handle->reordered_buffer, npix*handle->data_size);
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_unreorder_bytepack( xrif_t handle )
{
   int_fast8_t x1, x2;

   char * raw_buffer = handle->raw_buffer + handle->width*handle->height* handle->depth *handle->data_size;
   char * reordered_buffer = handle->reordered_buffer + handle->width*handle->height* handle->depth *handle->data_size;
   size_t npix = handle->width * handle->height * handle->depth * (handle->frames-1); 
   
   for(size_t pix=0; pix<handle->width*handle->height* handle->depth *handle->data_size; ++pix)
   {
      handle->raw_buffer[pix] = handle->reordered_buffer[pix];
   }
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      x2 = reordered_buffer[pix]; 
      x1 = reordered_buffer[npix+pix];
         
      if(x2 < 0)
      {
         if(x1 == -1) x1 = 0;
         else if(x1 == 0) x1 = -1;
      }
         
      raw_buffer[2*pix] = x2;
      raw_buffer[2*pix+1] = x1;
         
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif
      
   return XRIF_NOERROR;
}

xrif_error_t xrif_unreorder_bytepack_renibble( xrif_t handle )
{
   size_t npix = handle->width * handle->height * handle->depth * (handle->frames-1); 
   
   int16_t * raw_buffer = (int16_t*)(handle->raw_buffer + handle->width*handle->height* handle->depth *handle->data_size);
   unsigned char * reordered_buffer = (unsigned char *) handle->reordered_buffer + handle->width*handle->height* handle->depth *handle->data_size;
   unsigned char * reordered_buffer2 = (unsigned char*) reordered_buffer + npix;
   
   for(size_t pix=0; pix<handle->width*handle->height* handle->depth *handle->data_size; ++pix)
   {
      handle->raw_buffer[pix] = handle->reordered_buffer[pix];
   }
   
   size_t halfoff = ((double) npix)/2 + 0.5;
   size_t oneoff = 0;
   if(halfoff > npix/2) oneoff = 0;
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      uint16_t byte1 = 0;
      byte1 = reordered_buffer[pix];
      
      uint16_t nib1 =0;
      uint16_t nib2 = 0;
      if(pix % 2 == 0)
      {
         nib1 |= ((reordered_buffer2[pix/2+oneoff])) >> 4;  
         nib2 |= reordered_buffer2[pix/2+oneoff + halfoff] & 240; 
      }
      else
      {
         nib1 |= reordered_buffer2[pix/2] & 15;
         nib2 |= (reordered_buffer2[pix/2 + oneoff + halfoff] & 15) << 4;
      }
      
      byte1 |= (nib1 << 8);
      byte1 |= (nib2 << 8);
      
      
      unsigned int sbit = (byte1 & 1);

      unsigned int s = (byte1 >> 1);
      
      
      if(sbit == 1 && s == 0) 
      {
         raw_buffer[pix] = -32768;
      }
      else
      {
         raw_buffer[pix] = s*(1-2*sbit);
      }
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}

xrif_error_t xrif_unreorder_bitpack( xrif_t handle )
{
   size_t one_frame = handle->width*handle->height* handle->depth *handle->data_size;
   
   for(size_t pix=0; pix< one_frame; ++pix)
   {
      handle->raw_buffer[pix] = handle->reordered_buffer[pix];
   }
   
   int16_t * raw_buffer = (int16_t *) (handle->raw_buffer + one_frame);
   uint16_t * reordered_buffer = (uint16_t *) (handle->reordered_buffer + one_frame);
   
   size_t npix = handle->width * handle->height * handle->depth * (handle->frames-1); 

   memset(raw_buffer, 0, npix*2);
   
   //size_t stride = npix/16;
   size_t stride = (handle->reordered_buffer_size - one_frame)/16/2;
   //printf("strides: %ld %ld\n", npix/16, stride);
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix<npix; ++pix)
   {
      
      size_t sbyte = pix/16;
      size_t bit = pix % 16;

      char sbit = (reordered_buffer[sbyte] >> bit) & 1;
      
      for(int_fast8_t b=1; b<16;++b)
      {
         raw_buffer[pix] |= ((reordered_buffer[sbyte +  b*stride] >> bit) & 1) << (b-1);
      }
      
      //Check if sign change needed
      if(sbit == 1)
      {
         //Check for -0, which is actually -32768
         if(raw_buffer[pix] == 0) raw_buffer[pix] = -32768;
         else raw_buffer[pix] *= -1;
      }
      
      
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}



xrif_error_t xrif_compress( xrif_t handle )
{
   int method = handle->compress_method;
   
   if(method == 0) method = XRIF_COMPRESS_DEFAULT;
   
   switch( method )
   {
      case XRIF_COMPRESS_NONE:
         return xrif_compress_none(handle);
      case XRIF_COMPRESS_LZ4:
         return xrif_compress_lz4(handle);
      default:
         return XRIF_ERROR_NOTIMPL;
   }
      
}

xrif_error_t xrif_decompress( xrif_t handle )
{
   int method = handle->compress_method;
   
   if(method == 0) method = XRIF_COMPRESS_DEFAULT;
   
   switch( method )
   {
      case XRIF_COMPRESS_NONE:
         return xrif_decompress_none(handle);
      case XRIF_COMPRESS_LZ4:
         return xrif_decompress_lz4(handle);
      default:
         fprintf(stderr, "xrif_decompress: unknown compression method (%d)\n", method);
         return XRIF_ERROR_NOTIMPL;
   }
      
}

xrif_error_t xrif_compress_none( xrif_t handle )
{
   char *compressed_buffer;
   size_t compressed_size;
   
   if(handle->compress_on_raw) 
   {
      compressed_buffer = handle->raw_buffer;
      compressed_size = handle->raw_buffer_size;
   }
   else 
   {
      compressed_buffer = handle->compressed_buffer;
      compressed_size = handle->compressed_buffer_size;
   }
   
   //Make sure there is enough space
   if( compressed_size < handle->reordered_buffer_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }

   //Zero extra pixels
   if(compressed_size > handle->reordered_buffer_size)
   {
      ///\todo this can be just the xtra pixels
      memset(compressed_buffer, 0, compressed_size);
   }
   
   handle->compressed_size = handle->reordered_buffer_size;
   
   memcpy( compressed_buffer, handle->reordered_buffer, handle->compressed_size);
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_decompress_none( xrif_t handle )
{
   char *compressed_buffer;
   
   if(handle->compress_on_raw) 
   {
      compressed_buffer = handle->raw_buffer;
   }
   else 
   {
      compressed_buffer = handle->compressed_buffer;
   }
   
   //The reordered buffer must be big enough to old the compressed data
   if(handle->reordered_buffer_size < handle->compressed_size) 
   {
      fprintf(stderr, "xrif_decompress_none: reordered_buffer is too small (%ld < %ld).\n", handle->reordered_buffer_size, handle->compressed_size);
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   //If the reordered buffer is larger than needed to hold the compressed buffer, set extras to 0
   if(handle->reordered_buffer_size > handle->compressed_size)
   {
      ///\todo this can be just the extra bytes
      memset(handle->reordered_buffer, 0, handle->reordered_buffer_size);
   }

   //Now it's just a straight copy from compressed to reordered buffer.
   memcpy( handle->reordered_buffer, compressed_buffer, handle->compressed_size);
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_compress_lz4( xrif_t handle )
{
   char *compressed_buffer;
   size_t compressed_size;
   
   if(handle->compress_on_raw) 
   {
      compressed_buffer = handle->raw_buffer;
      compressed_size = handle->raw_buffer_size;
   }
   else 
   {
      compressed_buffer = handle->compressed_buffer;
      compressed_size = handle->compressed_buffer_size;
   }
   
   handle->compressed_size = LZ4_compress_fast ( handle->reordered_buffer, compressed_buffer, handle->reordered_buffer_size, compressed_size, handle->lz4_acceleration);
   
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_decompress_lz4( xrif_t handle )
{
   char *compressed_buffer;
   
   if(handle->compress_on_raw) 
   {
      compressed_buffer = handle->raw_buffer;
   }
   else 
   {
      compressed_buffer = handle->compressed_buffer;
   }
   
   int size_decomp = LZ4_decompress_safe (compressed_buffer, handle->reordered_buffer, handle->compressed_size, handle->reordered_buffer_size);

//    if(handle->reordered_buffer_size != size_decomp) 
//    {
//       fprintf(stderr, "xrif_decompress_lz4: size mismatch after decompression.\n");
//       return XRIF_ERROR_INVALID_SIZE;
//    }
   
   return XRIF_NOERROR;
}

double xrif_compression_ratio( xrif_t handle )
{
   return ((double)handle->compressed_size)/((double)handle->raw_size);
}

double xrif_encode_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->ts_compress_done, &handle->ts_difference_start);
}

double xrif_encode_rate( xrif_t handle )
{
   return ((double) handle->raw_size) /xrif_encode_time(handle);
}
   
double xrif_difference_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->ts_reorder_start, &handle->ts_difference_start);
}

double xrif_difference_rate( xrif_t handle )
{
   return ((double) handle->raw_size) / xrif_difference_time(handle);
}

double xrif_reorder_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->ts_compress_start, &handle->ts_reorder_start);
}

double xrif_reorder_rate( xrif_t handle )
{
   return ((double) handle->raw_size) /xrif_reorder_time(handle);
}

double xrif_compress_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->ts_compress_done, &handle->ts_compress_start);
}

double xrif_compress_rate( xrif_t handle )
{
   return ((double) handle->raw_size) /xrif_compress_time(handle);
}

size_t xrif_typesize( xrif_typecode_t type_code)
{
   switch( type_code )
   {
      case XRIF_TYPECODE_UINT8:
         return sizeof(uint8_t);
      case XRIF_TYPECODE_INT8:
         return sizeof(int8_t);
      case XRIF_TYPECODE_UINT16:
         return sizeof(uint16_t);
      case XRIF_TYPECODE_INT16:
         return sizeof(int16_t);
      case XRIF_TYPECODE_UINT32:
         return sizeof(uint32_t);
      case XRIF_TYPECODE_INT32:
         return sizeof(int32_t);
      case XRIF_TYPECODE_UINT64:
         return sizeof(uint64_t);
      case XRIF_TYPECODE_INT64:
         return sizeof(int64_t);
      case XRIF_TYPECODE_HALF:
         return sizeof(uint16_t);
      case XRIF_TYPECODE_FLOAT:
         return sizeof(float);
      case XRIF_TYPECODE_DOUBLE:
         return sizeof(double);
      case XRIF_TYPECODE_COMPLEX_FLOAT:
         return sizeof(float[2]);
      case XRIF_TYPECODE_COMPLEX_DOUBLE:
         return sizeof(double[2]);
      default:
         return 0;
   }
}

double xrif_ts_difference( struct timespec * ts1,
                           struct timespec * ts0
                         )
{
   return ((double)ts1->tv_sec) + ((double)ts1->tv_nsec)/1e9 - ((double)ts0->tv_sec) - ((double)ts0->tv_nsec)/1e9;
}

const char * xrif_difference_method_string( int diff_method )
{
   switch(diff_method)
   {
      case XRIF_DIFFERENCE_NONE:
         return "none";
      case XRIF_DIFFERENCE_PREVIOUS:
         return "previous";
      case XRIF_DIFFERENCE_FIRST:
         return "first";
      default:
         return "unknown";
   }
}

const char * xrif_reorder_method_string( int reorder_method )
{
   switch(reorder_method)
   {
      case XRIF_REORDER_NONE:
         return "none";
      case XRIF_REORDER_BYTEPACK:
         return "bytepack";
      case XRIF_REORDER_BYTEPACK_RENIBBLE:
         return "bytepack w/ renibble";
      case XRIF_REORDER_BITPACK:
         return "bitpack";
      default:
         return "unknown";
   }
}

const char * xrif_compress_method_string( int compress_method )
{
   switch(compress_method)
   {
      case XRIF_COMPRESS_NONE:
         return "none";
      case XRIF_COMPRESS_LZ4:
         return "LZ4";
      default:
         return "unknown";
   }
}
