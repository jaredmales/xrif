/** \file xrif_difference_pixel1.c
  * \brief Implementation of xrif pixel differencing
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

xrif_error_t xrif_difference_pixel1_sint8( xrif_t handle )
{
   for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int8_t * imoff = ((int8_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //First difference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int8_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 0; nn < handle->m_width-1; ++nn)
            {
               rowoff[handle->m_width - nn - 1] -= rowoff[handle->m_width - nn - 2];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

         //Now do first row
         for(size_t nn=0; nn < handle->m_height-1; ++nn)
         {
            size_t idx0 = (handle->m_height - nn - 1)*handle->m_width;
            size_t idx1 = (handle->m_height - nn - 2)*handle->m_width;

            imoff[idx0] -= imoff[idx1];
         }
      }
   }

   return XRIF_NOERROR;

} //xrif_difference_pixel1_sint8

xrif_error_t xrif_difference_pixel1_sint16( xrif_t handle )
{
   for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int16_t * imoff = ((int16_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //First difference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int16_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 0; nn < handle->m_width-1; ++nn)
            {
               rowoff[handle->m_width - nn - 1] -= rowoff[handle->m_width - nn - 2];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

         //Now do first row
         for(size_t nn=0; nn < handle->m_height-1; ++nn)
         {
            size_t idx0 = (handle->m_height - nn - 1)*handle->m_width;
            size_t idx1 = (handle->m_height - nn - 2)*handle->m_width;

            imoff[idx0] -= imoff[idx1];
         }
      }
   }  
   
   return XRIF_NOERROR;
   
} //xrif_difference_pixel1_sint16

xrif_error_t xrif_difference_pixel1_sint32( xrif_t handle )
{
for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int32_t * imoff = ((int32_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //First difference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int32_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 0; nn < handle->m_width-1; ++nn)
            {
               rowoff[handle->m_width - nn - 1] -= rowoff[handle->m_width - nn - 2];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

         //Now do first row
         for(size_t nn=0; nn < handle->m_height-1; ++nn)
         {
            size_t idx0 = (handle->m_height - nn - 1)*handle->m_width;
            size_t idx1 = (handle->m_height - nn - 2)*handle->m_width;

            imoff[idx0] -= imoff[idx1];
         }
      }
   }  
   
   return XRIF_NOERROR;

} //xrif_difference_pixel1_sint32

xrif_error_t xrif_difference_pixel1_sint64( xrif_t handle )
{
   for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int64_t * imoff = ((int64_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //First difference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int64_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 0; nn < handle->m_width-1; ++nn)
            {
               rowoff[handle->m_width - nn - 1] -= rowoff[handle->m_width - nn - 2];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

         //Now do first row
         for(size_t nn=0; nn < handle->m_height-1; ++nn)
         {
            size_t idx0 = (handle->m_height - nn - 1)*handle->m_width;
            size_t idx1 = (handle->m_height - nn - 2)*handle->m_width;

            imoff[idx0] -= imoff[idx1];
         }
      }
   }  
   
   return XRIF_NOERROR;
   
} //xrif_difference_pixel1_sint64


//Dispatch differencing w.r.t. previous according to type
xrif_error_t xrif_difference_pixel1( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel1", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if( handle->m_raw_buffer == NULL || handle->m_width*handle->m_height*handle->m_depth*handle->m_frames == 0 || handle->m_type_code == 0)
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel1", "handle not set up");
      return XRIF_ERROR_NOT_SETUP;
   }
      
   if(handle->m_raw_buffer_size < handle->m_width*handle->m_height*handle->m_depth*handle->m_frames)
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel1", "raw buffer size not sufficient");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT8 || handle->m_type_code == XRIF_TYPECODE_UINT8)
   {
      return xrif_difference_pixel1_sint8(handle);
   }
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_difference_pixel1_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      return xrif_difference_pixel1_sint32(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      return xrif_difference_pixel1_sint64(handle);
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel1", "pixel1 differencing not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
} //xrif_difference_pixel1

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// undifferencing
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


xrif_error_t xrif_undifference_pixel1_sint8( xrif_t handle )
{
   for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int8_t * imoff = ((int8_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;

         //First do first row
         for(size_t nn=1; nn < handle->m_height; ++nn)
         {
            size_t idx0 = (nn-1)*handle->m_width;
            size_t idx1 = (nn)*handle->m_width;

            imoff[idx1] += imoff[idx0];
         }

         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //Now undifference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int8_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 1; nn < handle->m_width; ++nn)
            {
               rowoff[nn] += rowoff[nn - 1];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

      }
   }  
   
   return XRIF_NOERROR;

}//xrif_undifference_pixel1_sint8

xrif_error_t xrif_undifference_pixel1_sint16( xrif_t handle )
{
   for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int16_t * imoff = ((int16_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;

         //First do first row
         for(size_t nn=1; nn < handle->m_height; ++nn)
         {
            size_t idx0 = (nn-1)*handle->m_width;
            size_t idx1 = (nn)*handle->m_width;

            imoff[idx1] += imoff[idx0];
         }

         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //Now undifference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int16_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 1; nn < handle->m_width; ++nn)
            {
               rowoff[nn] += rowoff[nn - 1];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

      }
   }  
   
   return XRIF_NOERROR;
   
}//xrif_undifference_pixel1_sint16

xrif_error_t xrif_undifference_pixel1_sint32( xrif_t handle )
{
   for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int32_t * imoff = ((int32_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;

         //First do first row
         for(size_t nn=1; nn < handle->m_height; ++nn)
         {
            size_t idx0 = (nn-1)*handle->m_width;
            size_t idx1 = (nn)*handle->m_width;

            imoff[idx1] += imoff[idx0];
         }

         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //Now undifference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int32_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 1; nn < handle->m_width; ++nn)
            {
               rowoff[nn] += rowoff[nn - 1];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

      }
   }  
   
   return XRIF_NOERROR;

   
} //xrif_undifference_pixel1_sint32

xrif_error_t xrif_undifference_pixel1_sint64( xrif_t handle )
{
   for(size_t n=0; n < handle->m_frames; ++n)
   {
      for(size_t kk=0; kk< handle->m_depth; ++kk)
      {
         //This offsets to the first pixel in the image.
         int64_t * imoff = ((int64_t*) handle->m_raw_buffer) + n*handle->m_width*handle->m_height*handle->m_depth + kk*handle->m_width*handle->m_height;

         //First do first row
         for(size_t nn=1; nn < handle->m_height; ++nn)
         {
            size_t idx0 = (nn-1)*handle->m_width;
            size_t idx1 = (nn)*handle->m_width;

            imoff[idx1] += imoff[idx0];
         }

         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->m_omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         //Now undifference across each column
         for(size_t cc = 0; cc < handle->m_height; ++cc)
         {
            int64_t * rowoff = imoff + cc*handle->m_width;
            
            for(size_t nn = 1; nn < handle->m_width; ++nn)
            {
               rowoff[nn] += rowoff[nn - 1];
            }   
         }

         #ifndef XRIF_NO_OMP
         }
         #endif

      }
   }  
   
   return XRIF_NOERROR;

   
}//xrif_undifference_pixel1_sint64


//Dispatch undifferencing w.r.t. previous according to type
xrif_error_t xrif_undifference_pixel1( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_undifference_pixel1", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if( handle->m_raw_buffer == NULL || handle->m_width*handle->m_height*handle->m_depth*handle->m_frames == 0 || handle->m_type_code == 0)
   {
      XRIF_ERROR_PRINT("xrif_undifference_pixel1", "handle not set up");
      return XRIF_ERROR_NOT_SETUP;
   }
      
   if(handle->m_raw_buffer_size < handle->m_width*handle->m_height*handle->m_depth*handle->m_frames)
   {
      XRIF_ERROR_PRINT("xrif_undifference_pixel1", "raw buffer size not sufficient");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   if(handle->m_type_code == XRIF_TYPECODE_INT8 || handle->m_type_code == XRIF_TYPECODE_UINT8)
   {
      return xrif_undifference_pixel1_sint8(handle);
   }
   if(handle->m_type_code == XRIF_TYPECODE_INT16 || handle->m_type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_undifference_pixel1_sint16(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT32 || handle->m_type_code == XRIF_TYPECODE_UINT32)
   {
      return xrif_undifference_pixel1_sint32(handle);
   }
   else if(handle->m_type_code == XRIF_TYPECODE_INT64 || handle->m_type_code == XRIF_TYPECODE_UINT64)
   {
      return xrif_undifference_pixel1_sint64(handle);
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel1", "pixel1 undifferencing not implemented for type");
      return XRIF_ERROR_NULLPTR;
   }
   
} //xrif_undifference_pixel1
