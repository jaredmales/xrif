/** \file xrif_difference_pixel.c
  * \brief Implementation of xrif first frame differencing
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



xrif_error_t xrif_difference_pixel_sint16( xrif_t handle )
{
   size_t npix = handle->width*handle->height;
   
   for(size_t n=0; n < handle->frames; ++n)
   {
      for(size_t kk=0; kk< handle->depth; ++kk)
      {
         int16_t * rboff = (int16_t*) handle->raw_buffer + n * npix * handle->depth + kk*npix;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         for(size_t nn = 0; nn < npix-1; ++nn)
         {
            rboff[npix - nn - 1] -= rboff[npix - nn - 2];
         }   
         
         #ifndef XRIF_NO_OMP
         }
         #endif
      }
   }  
   
   return XRIF_NOERROR;
   
} //xrif_difference_pixel_sint16

xrif_error_t xrif_difference_pixel_sint32( xrif_t handle )
{
   size_t npix = handle->width*handle->height;
   
   for(size_t n=0; n < handle->frames; ++n)
   {
      for(size_t kk=0; kk< handle->depth; ++kk)
      {
         int32_t * rboff = (int32_t*) handle->raw_buffer + n * npix * handle->depth + kk*npix;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         for(size_t nn = 0; nn < npix-1; ++nn)
         {
            rboff[npix - nn - 1] -= rboff[npix - nn - 2];
         }   
         
         #ifndef XRIF_NO_OMP
         }
         #endif
      }
   }  
   
   return XRIF_NOERROR;
   
} //xrif_difference_pixel_sint32

xrif_error_t xrif_difference_pixel_sint64( xrif_t handle )
{
   size_t npix = handle->width*handle->height;
   
   for(size_t n=0; n < handle->frames; ++n)
   {
      for(size_t kk=0; kk< handle->depth; ++kk)
      {
         int64_t * rboff = (int64_t*) handle->raw_buffer + n * npix * handle->depth + kk*npix;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif

         for(size_t nn = 0; nn < npix-1; ++nn)
         {
            rboff[npix - nn - 1] -= rboff[npix - nn - 2];
         }   
         
         #ifndef XRIF_NO_OMP
         }
         #endif
      }
   }  
   
   return XRIF_NOERROR;
   
} //xrif_difference_pixel_sint64


//Dispatch differencing w.r.t. previous according to type
xrif_error_t xrif_difference_pixel( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if( handle->raw_buffer == NULL || handle->width*handle->height*handle->depth*handle->frames == 0 || handle->type_code == 0)
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel", "handle not set up");
      return XRIF_ERROR_NOT_SETUP;
   }
      
   if(handle->raw_buffer_size < handle->width*handle->height*handle->depth*handle->frames)
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel", "raw buffer size not sufficient");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   if(handle->type_code == XRIF_TYPECODE_INT16 || handle->type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_difference_pixel_sint16(handle);
   }
   else if(handle->type_code == XRIF_TYPECODE_INT32 || handle->type_code == XRIF_TYPECODE_UINT32)
   {
      return xrif_difference_pixel_sint32(handle);
   }
   else if(handle->type_code == XRIF_TYPECODE_INT64 || handle->type_code == XRIF_TYPECODE_UINT64)
   {
      return xrif_difference_pixel_sint64(handle);
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel", "previous differencing not implemented for type");
      return XRIF_ERROR_NOTIMPL;
   }
} //xrif_difference_pixel

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// undifferencing
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



xrif_error_t xrif_undifference_pixel_sint16( xrif_t handle )
{
   size_t npix = handle->width*handle->height;
               
   for(int n=0; n < handle->frames; ++n)
   {
      for(int kk=0; kk< handle->depth; ++kk)
      {
         int16_t * rboff = (int16_t*)handle->raw_buffer + n * npix * handle->depth + kk*npix;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif
            
         for(int nn = 1; nn < npix; ++nn)
         {
            rboff[nn] += rboff[nn-1] ;
         }    
         
         #ifndef XRIF_NO_OMP
         }
         #endif
      }
   }
   
   return XRIF_NOERROR;
   
}//xrif_undifference_pixel_sint16

xrif_error_t xrif_undifference_pixel_sint32( xrif_t handle )
{
   size_t npix = handle->width*handle->height;
               
   for(int n=0; n < handle->frames; ++n)
   {
      for(int kk=0; kk< handle->depth; ++kk)
      {
         int32_t * rboff = (int32_t*)handle->raw_buffer + n * npix * handle->depth + kk*npix;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif
            
         for(int nn = 1; nn < npix; ++nn)
         {
            rboff[nn] += rboff[nn-1] ;
         }    
         
         #ifndef XRIF_NO_OMP
         }
         #endif
      }
   }
   
   return XRIF_NOERROR;
   
} //xrif_undifference_pixel_sint32

xrif_error_t xrif_undifference_pixel_sint64( xrif_t handle )
{
   size_t npix = handle->width*handle->height;
               
   for(int n=0; n < handle->frames; ++n)
   {
      for(int kk=0; kk< handle->depth; ++kk)
      {
         int64_t * rboff = (int64_t*)handle->raw_buffer + n * npix * handle->depth + kk*npix;
         
         #ifndef XRIF_NO_OMP
         #pragma omp parallel if (handle->omp_parallel > 0) 
         {
         #endif
   
         #ifndef XRIF_NO_OMP
         #pragma omp for
         #endif
            
         for(int nn = 1; nn < npix; ++nn)
         {
            rboff[nn] += rboff[nn-1] ;
         }    
         
         #ifndef XRIF_NO_OMP
         }
         #endif
      }
   }
   
   return XRIF_NOERROR;
   
}//xrif_undifference_pixel_sint64


//Dispatch undifferencing w.r.t. previous according to type
xrif_error_t xrif_undifference_pixel( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_undifference_pixel", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if( handle->raw_buffer == NULL || handle->width*handle->height*handle->depth*handle->frames == 0 || handle->type_code == 0)
   {
      XRIF_ERROR_PRINT("xrif_undifference_pixel", "handle not set up");
      return XRIF_ERROR_NOT_SETUP;
   }
      
   if(handle->raw_buffer_size < handle->width*handle->height*handle->depth*handle->frames)
   {
      XRIF_ERROR_PRINT("xrif_undifference_pixel", "raw buffer size not sufficient");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }
   
   if(handle->type_code == XRIF_TYPECODE_INT16 || handle->type_code == XRIF_TYPECODE_UINT16)
   {
      return xrif_undifference_pixel_sint16(handle);
   }
   else if(handle->type_code == XRIF_TYPECODE_INT32 || handle->type_code == XRIF_TYPECODE_UINT32)
   {
      return xrif_undifference_pixel_sint32(handle);
   }
   else if(handle->type_code == XRIF_TYPECODE_INT64 || handle->type_code == XRIF_TYPECODE_UINT64)
   {
      return xrif_undifference_pixel_sint64(handle);
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_difference_pixel", "previous undifferencing not implemented for type");
      return XRIF_ERROR_NULLPTR;
   }
} //xrif_undifference_pixel
