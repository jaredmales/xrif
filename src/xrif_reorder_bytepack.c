/** \file xrif_reorder_bytepack.c
  * \brief Implementation of xrif bytepack reordering
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
//  bytepack reordering
//--------------------------------------------------------------------

/// Perform bytepack reodering for signed 16 bit ints
/** 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_reorder_bytepack_sint16( xrif_t handle )
{
   size_t one_frame, npix;
 
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_sint16", "can not use a null pointer");
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

   if( handle->m_raw_buffer_size < one_frame + npix*handle->m_data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   if( handle->m_reordered_buffer_size < one_frame + npix*handle->m_data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }

   char * m_raw_buffer = handle->m_raw_buffer + one_frame ;
   char * m_reordered_buffer = handle->m_reordered_buffer + one_frame;
   char * m_reordered_buffer2 = m_reordered_buffer + npix;

   ///\todo is this actually necessary, and can this can be just the extra pixels?
   //Zero the reordered buffer.
   //memset(handle->m_reordered_buffer, 0, handle->m_reordered_buffer_size); 
   
   //Set the first part of the reordered buffer to the first frame (always the reference frame)
   memcpy(handle->m_reordered_buffer,handle->m_raw_buffer, one_frame);
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->m_omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      //Note: a lookup table for this was found to be 2x slower than the following algorithm.
      int_fast8_t x2 = m_raw_buffer[2*pix];
      int_fast8_t x1 = m_raw_buffer[2*pix+1];

      if(x2 < 0)
      {
         if(x1 == -1)  x1 = 0;
         else if(x1 == 0) x1 = -1;
      }
         
      m_reordered_buffer[pix] = x2; 
      m_reordered_buffer2[pix] = x1;
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}

/// Perform bytepack reodering for signed 32 bit ints
/** 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * 
  * \ingroup xrif_reorder_bytepack
  */
xrif_error_t xrif_reorder_bytepack_sint32( xrif_t handle )
{
   size_t one_frame, npix;
 
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack_sint32", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_difference_method != XRIF_DIFFERENCE_FIRST && handle->m_difference_method != XRIF_DIFFERENCE_PREVIOUS0)
   {
      one_frame = 0;
      npix = handle->m_width * handle->m_height * handle->m_depth * handle->m_frames;  //pixels not bytes
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
   
   if( handle->m_reordered_buffer_size < one_frame + npix*handle->m_data_size )
   {
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }

   char * m_raw_buffer = handle->m_raw_buffer + one_frame ;
   char * m_reordered_buffer = handle->m_reordered_buffer + one_frame;
   char * m_reordered_buffer2 = m_reordered_buffer + npix;
   char * m_reordered_buffer3 = m_reordered_buffer2 + npix;
   char * m_reordered_buffer4 = m_reordered_buffer3 + npix;

   //transfer the reference frame if it's not reordered
   if(one_frame > 0)  memcpy(handle->m_reordered_buffer,handle->m_raw_buffer, one_frame);
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->m_omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      //Note: a lookup table for this was found to be 2x slower than the following algorithm.
      int_fast8_t x4 = m_raw_buffer[2*pix];
      int_fast8_t x3 = m_raw_buffer[2*pix+1];
      int_fast8_t x2 = m_raw_buffer[2*pix+2];
      int_fast8_t x1 = m_raw_buffer[2*pix+3];

      if(x4 < 0)
      {
         if(x1 == -1)  x1 = 0;
         else if(x1 == 0) x1 = -1;
      }
         
      m_reordered_buffer[pix] = x4;
      m_reordered_buffer2[pix] = x3;
      m_reordered_buffer3[pix] = x2; 
      m_reordered_buffer4[pix] = x1;
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif

   return XRIF_NOERROR;
}

//Dispatch bytepack reordering according to type
xrif_error_t xrif_reorder_bytepack( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_reorder_bytepack_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      return xrif_reorder_bytepack_sint32(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack", "bytepack reordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_reorder_bytepack", "bytepack reordering not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
   
}



//--------------------------------------------------------------------
//  bytepack unreodering
//--------------------------------------------------------------------


/// Perform bytepack unreodering for signed 16 bit ints
/** 
  * \todo this does not actually perform any size checks, but should.
  * 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_unreorder_bytepack_sint16( xrif_t handle )
{
   size_t one_frame, npix;
   
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack_sint16", "can not use a null pointer");
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
   
   char * m_raw_buffer = handle->m_raw_buffer + one_frame;
   char * m_reordered_buffer = handle->m_reordered_buffer + one_frame;
      
   for(size_t pix=0; pix<one_frame; ++pix)
   {
      handle->m_raw_buffer[pix] = handle->m_reordered_buffer[pix];
   }
   
   #ifndef XRIF_NO_OMP
   #pragma omp parallel if (handle->m_omp_parallel > 0) 
   {
   #pragma omp for
   #endif
   for(size_t pix = 0; pix < npix; ++pix)
   {
      int_fast8_t x2 = m_reordered_buffer[pix]; 
      int_fast8_t x1 = m_reordered_buffer[npix+pix];
         
      if(x2 < 0)
      {
         if(x1 == -1) x1 = 0;
         else if(x1 == 0) x1 = -1;
      }
         
      m_raw_buffer[2*pix] = x2;
      m_raw_buffer[2*pix+1] = x1;
         
   }
   
   #ifndef XRIF_NO_OMP
   }
   #endif
      
   return XRIF_NOERROR;
}

//Dispatch bytepack unreordering according to type
xrif_error_t xrif_unreorder_bytepack( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_unreorder_bytepack_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack", "bytepack unreordering not implemented for 32 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack", "bytepack unreordering not implemented for 64 bit ints");
      return XRIF_ERROR_NOTIMPL;
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_unreorder_bytepack", "bytepack unreordering not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
   
}//xrif_error_t xrif_unreorder_bytepack( xrif_t handle )

