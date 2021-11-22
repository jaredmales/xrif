/** \file xrif_reorder_bitpack.c
  * \brief Implementation of xrif bitpack reordering
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_files
  */

/* This file is part of the xrif library.

Copyright (c) 2021 The Arizona Board of Regents on behalf of The
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

//--------------------------------------------------------------------
//  bitpack reordering
//--------------------------------------------------------------------

/// Perform bitpack reordering for signed 16 bit ints
/** 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_reorder_bitpack_sint16( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_sint16", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   //The lookup tables (static arrays)
   #include "bit_to_position.inc"
   #include "set_bits.inc"
   
   size_t one_frame, npix;
   
   
   if(handle->m_difference_method != XRIF_DIFFERENCE_FIRST && handle->m_difference_method != XRIF_DIFFERENCE_PREVIOUS0)
   {
      one_frame = 0;
      npix = handle->m_width * handle->m_height * handle->m_depth * handle->m_frames;
   }
   else //For the deprecated frame-to-frame methods we don't do the first frame.
   {
      one_frame = handle->m_width*handle->m_height* handle->m_depth *handle->m_data_size; //bytes
      npix = handle->m_width * handle->m_height * handle->m_depth * (handle->m_frames-1); //pixels not bytes
   }
   
   for(size_t pix=0; pix < one_frame; ++pix)
   {
      handle->m_reordered_buffer[pix] = handle->m_raw_buffer[pix];
   }
   
   int16_t * m_raw_buffer = (int16_t *) (handle->m_raw_buffer + one_frame);
   uint16_t * m_reordered_buffer = (uint16_t *) (handle->m_reordered_buffer + one_frame);
   
   memset( (char *) m_reordered_buffer, 0, handle->m_reordered_buffer_size - one_frame);

   size_t stride = (handle->m_reordered_buffer_size - one_frame)/16/2; //stride in 16-bit pixels, not bytes
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->m_omp_parallel > 0) 
   {
   #pragma omp for
   #endif
      
  
   for(size_t pix = 0; pix<npix; ++pix)
   {
      size_t sbyte = pix/16; //This is the starting byte for this pixel
      int_fast8_t bit = pix % 16; //This is the bit position for this pixel
     
      
      //--- Reordering sign bit [left in to document]
      //int16_t s = m_raw_buffer[pix];
      //int8_t sbit = (s < 0);
      //s *= (1-2*sbit); //make positive
      //uint16_t us = ( (s) << 1) | sbit; //shift by 1, putting sign bit in highest entropy spot
      
      //--- Simpler way to reorder, this is equivalent to above
      int8_t sbit = (m_raw_buffer[pix] < 0);
      uint16_t us = 2*m_raw_buffer[pix]*(1-2*sbit)+sbit;
     

      //---- This is the basic algorithm, without lookup tables:
      /*
      for(int b=0; b < 16; ++b)
      {
         m_reordered_buffer[sbyte + b*stride] += (((us >> b) & 1) << bit);
      }
      */
      
      //---- A full lookup table version
      //Attempt with lookup table is slower, leaving this in to document this, possibly for future defines:
      //uint16_t us = left_shift_one[*((uint16_t *) &m_raw_buffer[pix])];

      //---- A faster lookup table version:
      size_t sbyte8 = sbyte + 8*stride;
      size_t st0 = ((unsigned char *) &us)[0]*16*8 + bit*8;
      size_t st1 = ((unsigned char *) &us)[1]*16*8 + bit*8;
      
      for(int_fast8_t b = 0; b < 8; ++b)
      {  
         m_reordered_buffer[sbyte +  b*stride] += bit_to_position[st0 + b] ;
      }
      if(((unsigned char *) &us)[1] == 0) continue;
      
      //Get the number of set bits, and for this second byte we only process those bits.
      unsigned char nbits2 = set_bits[((unsigned char *) &us)[1]*9];
      const unsigned char* bits2 = &set_bits[((unsigned char *) &us)[1]*9 + 1];
      for(int_fast8_t b = 0; b < nbits2; ++b)
      {      
         m_reordered_buffer[sbyte8 +  bits2[b]*stride] +=  bit_to_position[st1 + bits2[b]];
      }
   }
   #ifndef XRIF_NO_OMP
   }
   #endif
   
   return XRIF_NOERROR;
}


//Dispatch bitpack reordering according to type
xrif_error_t xrif_reorder_bitpack( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_reorder_bitpack", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_reorder_bitpack_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      XRIF_ERROR_PRINT("xrif_reorder_bitpack", "bitpack reordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      XRIF_ERROR_PRINT("xrif_reorder_bitpack", "bitpack reordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_reorder_bitpack", "bitpack reordering not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
   
}



//--------------------------------------------------------------------
//  bitpack unreodering
//--------------------------------------------------------------------


/// Perform bitpack unreodering for signed 16 bit ints
/** 
  * \todo this does not actually perform any size checks, but should.
  * 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * \ingroup xrif_reorder_bitpack
  */ 
xrif_error_t xrif_unreorder_bitpack_sint16( xrif_t handle )
{   
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bitpack_sint16", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   size_t one_frame, npix;
   
   if(handle->m_difference_method != XRIF_DIFFERENCE_FIRST && handle->m_difference_method != XRIF_DIFFERENCE_PREVIOUS0)
   {
      one_frame = 0;
      npix = handle->m_width * handle->m_height * handle->m_depth * handle->m_frames;
   }
   else //For the deprecated frame-to-frame methods we don't do the first frame.
   {
      one_frame = handle->m_width*handle->m_height* handle->m_depth *handle->m_data_size; //bytes
      npix = handle->m_width * handle->m_height * handle->m_depth * (handle->m_frames-1); //pixels not bytes
   }
   
   for(size_t pix=0; pix< one_frame; ++pix)
   {
      handle->m_raw_buffer[pix] = handle->m_reordered_buffer[pix];
   }
   
   int16_t * m_raw_buffer = (int16_t *) (handle->m_raw_buffer + one_frame);
   uint16_t * m_reordered_buffer = (uint16_t *) (handle->m_reordered_buffer + one_frame);
   
   memset(m_raw_buffer, 0, npix*2);
   
   //size_t stride = npix/16;
   size_t stride = (handle->m_reordered_buffer_size - one_frame)/16/2;
   //printf("strides: %ld %ld\n", npix/16, stride);
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->m_omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix<npix; ++pix)
   {
      
      size_t sbyte = pix/16;
      size_t bit = pix % 16;

      char sbit = (m_reordered_buffer[sbyte] >> bit) & 1;
      
      for(int_fast8_t b=1; b<16;++b)
      {
         m_raw_buffer[pix] |= ((m_reordered_buffer[sbyte +  b*stride] >> bit) & 1) << (b-1);
      }
      
      //Check if sign change needed
      if(sbit == 1)
      {
         //Check for -0, which is actually -32768
         if(m_raw_buffer[pix] == 0) m_raw_buffer[pix] = -32768;
         else m_raw_buffer[pix] *= -1;
      }
      
      
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}

//Dispatch bitpack unreordering according to type
xrif_error_t xrif_unreorder_bitpack( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bitpack", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_unreorder_bitpack_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bitpack", "bitpack unreordering not implemented for 32 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bitpack", "bitpack unreordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack", "bytepack unreordering not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
   
}//xrif_error_t xrif_unreorder_bitpack( xrif_t handle )

