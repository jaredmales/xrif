/** \file xrif_reorder_bytepack_renibble.c
  * \brief Implementation of xrif bytepack_renibble reordering
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
//  bytepack_renibble reordering
//--------------------------------------------------------------------

/// Perform bytepack_renibble reordering for signed 16 bit ints
/** 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_reorder_bytepack_renibble_sint16( xrif_t handle )
{
   size_t one_frame, npix;
 
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_sint16", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
      //The lookup table, a static array
   #include "bitshift_and_nibbles.inc"
      
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
   
   
   if( handle->m_raw_buffer_size < one_frame + npix*handle->m_data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   //The reordered buffer must be frames+1 big to be sure it can handle odd sizes. 
   if( handle->m_reordered_buffer_size < xrif_min_reordered_size(handle) )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   //Zero the reordered buffer.
   memset(handle->m_reordered_buffer,0, xrif_min_reordered_size(handle));
   
   int16_t * m_raw_buffer = (int16_t*)(handle->m_raw_buffer + one_frame);
   
   //Get pointer that starts one image into the handle->m_reordered_buffer.  This area is 2*npix bytes long
   unsigned char * m_reordered_buffer = (unsigned char *) handle->m_reordered_buffer + one_frame;
   
   //Get point that starts halfway through, splitting at npix bytes
   unsigned char * m_reordered_buffer2 = (unsigned char*) m_reordered_buffer + npix; 
   
   //Set the first part of the reordered buffer to the first frame (always the reference frame)
   /*for(size_t pix=0; pix< one_frame; ++pix)
   {
      handle->m_reordered_buffer[pix] = handle->m_raw_buffer[pix];
   }*/
   memcpy(handle->m_reordered_buffer, handle->m_raw_buffer, one_frame);
   
   //Corrections necessary to handle odd numbers
   size_t halfoff = ((double) npix)/2.0 + 0.5;
   size_t oneoff = 0;
   if(halfoff > npix/2) oneoff = 0;
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->m_omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      /* This block of commented code is left in to document the algorithm implemented in the lookup table.
       * And maybe we'll implement some defines to avoid lookup tables . . .
       */
      
      /*
      int_fast16_t s = m_raw_buffer[pix]; //Get the first 2 bytes
      int_fast16_t sbit = (s < 0); //and the signbit

      s *= (1-2*sbit); //make it positive
      
      uint16_t us = ( (s) << 1) | sbit; //This moves the sign bit
       
      m_reordered_buffer[pix] = ((char *)(&us))[0]; //store first byte, which includes the sign bit?

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
      
      m_reordered_buffer2[pix/2] |= nib1;
      m_reordered_buffer2[pix/2 + oneoff + halfoff] |= nib2;
      */
      /*
      Here we use a lookup table calculated according to the above algorithm:
      */

      const unsigned char * bsn = &bitshift_and_nibbles[ ((uint16_t) m_raw_buffer[pix]) * 6 + (pix&1)*3];
      m_reordered_buffer[pix] = (char) bsn[0];
      m_reordered_buffer2[pix/2] += bsn[1];
      m_reordered_buffer2[pix/2 + oneoff + halfoff] += bsn[2];

   }
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}


//Dispatch bytepack_renibble reordering according to type
xrif_error_t xrif_reorder_bytepack_renibble( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_renibble", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_reorder_bytepack_renibble_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_renibble", "bytepack_renibble reordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_renibble", "bytepack_renibble reordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_renibble", "bytepack_renibble reordering not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
   
}



//--------------------------------------------------------------------
//  bytepack_renibble unreodering
//--------------------------------------------------------------------


/// Perform bytepack_renibble unreodering for signed 16 bit ints
/** 
  * \todo this does not actually perform any size checks, but should.
  * 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * \ingroup xrif_reorder_bytepack_renibble
  */ 
xrif_error_t xrif_unreorder_bytepack_renibble_sint16( xrif_t handle )
{
   size_t one_frame, npix;
   
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack_renibble_sint16", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
      
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
   
   int16_t * m_raw_buffer = (int16_t*)(handle->m_raw_buffer + one_frame);
   unsigned char * m_reordered_buffer = (unsigned char *) handle->m_reordered_buffer + one_frame;
   unsigned char * m_reordered_buffer2 = (unsigned char*) m_reordered_buffer + npix;
   
   for(size_t pix=0; pix<one_frame; ++pix)
   {
      handle->m_raw_buffer[pix] = handle->m_reordered_buffer[pix];
   }
   
   size_t halfoff = ((double) npix)/2 + 0.5;
   size_t oneoff = 0;
   if(halfoff > npix/2) oneoff = 0;
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->m_omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      uint16_t byte1 = 0;
      byte1 = m_reordered_buffer[pix];
      
      uint16_t nib1 =0;
      uint16_t nib2 = 0;
      if(pix % 2 == 0)
      {
         nib1 |= ((m_reordered_buffer2[pix/2+oneoff])) >> 4;  
         nib2 |= m_reordered_buffer2[pix/2+oneoff + halfoff] & 240; 
      }
      else
      {
         nib1 |= m_reordered_buffer2[pix/2] & 15;
         nib2 |= (m_reordered_buffer2[pix/2 + oneoff + halfoff] & 15) << 4;
      }
      
      byte1 |= (nib1 << 8);
      byte1 |= (nib2 << 8);
      
      
      unsigned int sbit = (byte1 & 1);

      unsigned int s = (byte1 >> 1);
      
      
      if(sbit == 1 && s == 0) 
      {
         m_raw_buffer[pix] = -32768;
      }
      else
      {
         m_raw_buffer[pix] = s*(1-2*sbit);
      }
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}

//Dispatch bytepack_renibble unreordering according to type
xrif_error_t xrif_unreorder_bytepack_renibble( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack_renibble", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_unreorder_bytepack_renibble_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack_renibble", "bytepack_renibble unreordering not implemented for 32 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack_renibble", "bytepack_renibble unreordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack", "bytepack unreordering not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
   
}//xrif_error_t xrif_unreorder_bytepack_renibble( xrif_t handle )

