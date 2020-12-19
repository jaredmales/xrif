
/* This file is part of the xrif library.

Copyright (c) 2019, 2020 The Arizona Board of Regents on behalf of The
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

#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "../src/xrif.h"

#include "randutils.h"

#ifndef XRIF_TEST_TRIALS
   #define XRIF_TEST_TRIALS (100)
#endif


/************************************************************/
/* Fuzz testing differencing with the previous method
/************************************************************/


int ws[] = {2,4,8,21, 33, 47, 64}; //widths of images
int hs[] = {2,4,8,21, 33, 47, 64}; //heights of images
int ps[] = {1,2,4,5,27,63,64}; //planes of the cube
 

// Verify that xrif difference/un-difference cycle using previous image works with white noise
// For int16_t
START_TEST (diff_previous_int16_white)
{
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   /*int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};*/

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   for(int q=0; q < XRIF_TEST_TRIALS; ++q)
   {
      for(int w =0; w < sizeof(ws)/sizeof(ws[0]); ++w)
      {
         for(int h=0; h < sizeof(hs)/sizeof(hs[0]); ++h)
         {
            for(int p=0; p< sizeof(ps)/sizeof(ps[0]); ++p)
            {
               
               rv = xrif_set_size(hand, ws[w], hs[h], 1, ps[p], XRIF_TYPECODE_INT16);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_allocate_raw(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               rv = xrif_allocate_reordered(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               int16_t * buffer = (int16_t *) hand->raw_buffer;
               rv = fill_int16_white( buffer, hand->width*hand->height*hand->frames);
               ck_assert( rv == 0 );
               
               int16_t * compbuff = (int16_t *) malloc( hand->width*hand->height*hand->frames*sizeof(int16_t));
               memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(int16_t));
            
               xrif_difference_previous_sint16(hand);
               xrif_undifference_previous_sint16(hand);
               
               int neq = 0;
               for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
               {
                  if(buffer[i] != compbuff[i]) 
                  {
                     ++neq;
                  }
               }
               ck_assert( neq == 0 );
               
               free(compbuff);
               xrif_reset(hand);
            }//p
         }//h
      }//w
   }//q
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST

// Verify that xrif difference/un-difference cycle using previous image works with white noise
// For uint16_t
START_TEST (diff_previous_uint16_white)
{
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   /*int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};*/

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   for(int q=0; q < XRIF_TEST_TRIALS; ++q)
   {
      for(int w =0; w < sizeof(ws)/sizeof(ws[0]); ++w)
      {
         for(int h=0; h < sizeof(hs)/sizeof(hs[0]); ++h)
         {
            for(int p=0; p< sizeof(ps)/sizeof(ps[0]); ++p)
            {
               
               rv = xrif_set_size(hand, ws[w], hs[h], 1, ps[p], XRIF_TYPECODE_UINT16);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_allocate_raw(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               rv = xrif_allocate_reordered(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               uint16_t * buffer = (uint16_t *) hand->raw_buffer;
               rv = fill_uint16_white( buffer, hand->width*hand->height*hand->frames);
               ck_assert( rv == 0 );
               
               uint16_t * compbuff = (uint16_t *) malloc( hand->width*hand->height*hand->frames*sizeof(uint16_t));
               memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(uint16_t));
            
               xrif_difference_previous_sint16(hand);
               xrif_undifference_previous_sint16(hand);
               
               int neq = 0;
               for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
               {
                  if(buffer[i] != compbuff[i]) 
                  {
                     ++neq;
                  }
               }
               ck_assert( neq == 0 );
               
               free(compbuff);
               xrif_reset(hand);
            }//p
         }//h
      }//w
   }//q
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST

// Verify that xrif difference/un-difference cycle using previous image works with white noise
// For int32_t
START_TEST (diff_previous_int32_white)
{
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   /*int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};*/

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   for(int q=0; q < XRIF_TEST_TRIALS; ++q)
   {
      for(int w =0; w < sizeof(ws)/sizeof(ws[0]); ++w)
      {
         for(int h=0; h < sizeof(hs)/sizeof(hs[0]); ++h)
         {
            for(int p=0; p< sizeof(ps)/sizeof(ps[0]); ++p)
            {
               
               rv = xrif_set_size(hand, ws[w], hs[h], 1, ps[p], XRIF_TYPECODE_INT32);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_allocate_raw(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               rv = xrif_allocate_reordered(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               int32_t * buffer = (int32_t *) hand->raw_buffer;
               rv = fill_int32_white( buffer, hand->width*hand->height*hand->frames);
               ck_assert( rv == 0 );
               
               int32_t * compbuff = (int32_t *) malloc( hand->width*hand->height*hand->frames*sizeof(int32_t));
               memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(int32_t));
            
               xrif_difference_previous_sint32(hand);
               xrif_undifference_previous_sint32(hand);
               
               int neq = 0;
               for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
               {
                  if(buffer[i] != compbuff[i]) 
                  {
                     ++neq;
                  }
               }
               ck_assert( neq == 0 );
               
               free(compbuff);
               xrif_reset(hand);
            }//p
         }//h
      }//w
   }//q
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST

// Verify that xrif difference/un-difference cycle using previous image works with white noise
// For uint16_t
START_TEST (diff_previous_uint32_white)
{
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   /*int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};*/

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   for(int q=0; q < XRIF_TEST_TRIALS; ++q)
   {
      for(int w =0; w < sizeof(ws)/sizeof(ws[0]); ++w)
      {
         for(int h=0; h < sizeof(hs)/sizeof(hs[0]); ++h)
         {
            for(int p=0; p< sizeof(ps)/sizeof(ps[0]); ++p)
            {
               
               rv = xrif_set_size(hand, ws[w], hs[h], 1, ps[p], XRIF_TYPECODE_UINT32);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_allocate_raw(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               rv = xrif_allocate_reordered(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               uint32_t * buffer = (uint32_t *) hand->raw_buffer;
               rv = fill_uint32_white( buffer, hand->width*hand->height*hand->frames);
               ck_assert( rv == 0 );
               
               uint32_t * compbuff = (uint32_t *) malloc( hand->width*hand->height*hand->frames*sizeof(uint32_t));
               memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(uint32_t));
            
               xrif_difference_previous_sint32(hand);
               xrif_undifference_previous_sint32(hand);
               
               int neq = 0;
               for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
               {
                  if(buffer[i] != compbuff[i]) 
                  {
                     ++neq;
                  }
               }
               ck_assert( neq == 0 );
               
               free(compbuff);
               xrif_reset(hand);
            }//p
         }//h
      }//w
   }//q
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST


// Verify that xrif difference/un-difference cycle using previous image works with white noise
// For int64_t
START_TEST (diff_previous_int64_white)
{
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   /*int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};*/

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   for(int q=0; q < XRIF_TEST_TRIALS; ++q)
   {
      for(int w =0; w < sizeof(ws)/sizeof(ws[0]); ++w)
      {
         for(int h=0; h < sizeof(hs)/sizeof(hs[0]); ++h)
         {
            for(int p=0; p< sizeof(ps)/sizeof(ps[0]); ++p)
            {
               
               rv = xrif_set_size(hand, ws[w], hs[h], 1, ps[p], XRIF_TYPECODE_INT64);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_allocate_raw(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               rv = xrif_allocate_reordered(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               int64_t * buffer = (int64_t *) hand->raw_buffer;
               rv = fill_int64_white( buffer, hand->width*hand->height*hand->frames);
               ck_assert( rv == 0 );
               
               int64_t * compbuff = (int64_t *) malloc( hand->width*hand->height*hand->frames*sizeof(int64_t));
               memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(int64_t));
            
               xrif_difference_previous_sint64(hand);
               xrif_undifference_previous_sint64(hand);
               
               int neq = 0;
               for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
               {
                  if(buffer[i] != compbuff[i]) 
                  {
                     ++neq;
                  }
               }
               ck_assert( neq == 0 );
               
               free(compbuff);
               xrif_reset(hand);
            }//p
         }//h
      }//w
   }//q
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST

// Verify that xrif difference/un-difference cycle using previous image works with white noise
// For uint64_t
START_TEST (diff_previous_uint64_white)
{
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   for(int q=0; q < XRIF_TEST_TRIALS; ++q)
   {
      for(int w =0; w < sizeof(ws)/sizeof(ws[0]); ++w)
      {
         for(int h=0; h < sizeof(hs)/sizeof(hs[0]); ++h)
         {
            for(int p=0; p< sizeof(ps)/sizeof(ps[0]); ++p)
            {
               
               rv = xrif_set_size(hand, ws[w], hs[h], 1, ps[p], XRIF_TYPECODE_UINT64);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_allocate_raw(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               rv = xrif_allocate_reordered(hand);
               ck_assert( rv == XRIF_NOERROR );
               
               uint64_t * buffer = (uint64_t *) hand->raw_buffer;
               rv = fill_uint64_white( buffer, hand->width*hand->height*hand->frames);
               ck_assert( rv == 0 );
               
               uint64_t * compbuff = (uint64_t *) malloc( hand->width*hand->height*hand->frames*sizeof(uint64_t));
               memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(uint64_t));
            
               xrif_difference_previous_sint64(hand);
               xrif_undifference_previous_sint64(hand);
               
               int neq = 0;
               for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
               {
                  if(buffer[i] != compbuff[i]) 
                  {
                     ++neq;
                  }
               }
               ck_assert( neq == 0 );
               
               free(compbuff);
               xrif_reset(hand);
            }//p
         }//h
      }//w
   }//q
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST

Suite * whitenoise_suite(void)
{
    Suite *s;
    TCase *tc_core16, *tc_core32, *tc_core64;

    s = suite_create("White Noise - Difference Previous");

    /* 16-bit Core test case */
    tc_core16 = tcase_create("16 bit white noise");

    tcase_set_timeout(tc_core16, 1e9);
    
    tcase_add_test(tc_core16, diff_previous_int16_white);
    tcase_add_test(tc_core16, diff_previous_uint16_white);
    
    /* 32-bit Core test case */
    tc_core32 = tcase_create("32 bit white noise");

    tcase_set_timeout(tc_core32, 1e9);
    
    tcase_add_test(tc_core32, diff_previous_int32_white);
    tcase_add_test(tc_core32, diff_previous_uint32_white);
    
    suite_add_tcase(s, tc_core32);

    /* 64-bit Core test case */
    tc_core64 = tcase_create("64 bit white noise");

    tcase_set_timeout(tc_core64, 1e9);
    
    tcase_add_test(tc_core64, diff_previous_int64_white);
    tcase_add_test(tc_core64, diff_previous_uint64_white);
    
    suite_add_tcase(s, tc_core64);
    
    return s;
}

int main()
{
   int number_failed;
   Suite *s;
   SRunner *sr;

   s = whitenoise_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   number_failed = srunner_ntests_failed(sr);
   srunner_free(sr);
   
   return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
   
}

