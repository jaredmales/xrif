
/* This file is part of the xrif library.

Copyright (c) 2019, The Arizona Board of Regents on behalf of The
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

START_TEST (diff_int16_white)
{
   //Verify that xrif difference/un-difference cycle works with white noise
   //For int16_t
   
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
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
            ck_assert( buffer != NULL);
            
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               buffer[i] = SHRT_MIN + (((double) rand())/RAND_MAX)*(SHRT_MAX-SHRT_MIN);
            }
            
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
            xrif_destroy_handle(hand);
         }
      }
   }
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
   
  
   
}
END_TEST


START_TEST (diff_uint16_white)
{
   //Verify that uxrif difference/un-difference cycle works with white noise
   //For int16_t
   
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
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
            
            
            uint16_t * buffer = (uint16_t *) hand->raw_buffer;
            ck_assert( buffer != NULL);
            
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               buffer[i] = (((double) rand())/RAND_MAX)*(USHRT_MAX);
            }
            
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
            xrif_destroy_handle(hand);
         }
      }
   }
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
   
  
   
}
END_TEST


START_TEST (reorder_bytepack_renibble_int16_white)
{
   //Verify that xrif reorder-unreorder cycle works for signed 16 bit integers
   
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};
   
   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   int fail = 0;
   for(int q=0; q< 100; ++q)
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
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               buffer[i] = SHRT_MIN  + (((double) rand())/RAND_MAX)*(SHRT_MAX-SHRT_MIN);
            }
   
            int16_t * compbuff = (int16_t *) malloc( hand->width*hand->height*hand->frames*sizeof(int16_t));
            memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(int16_t));
   
            xrif_difference_previous_sint16(hand);
            xrif_reorder_bytepack_renibble(hand);
            
            xrif_unreorder_bytepack_renibble(hand);
            xrif_undifference_previous_sint16(hand);
            
            
            int neq = 0;
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               if(buffer[i] != compbuff[i]) 
               {
                  /*if(i > 25)
                  fprintf(stderr, "err: %d %d %d\n", compbuff[i-25], compbuff[i], buffer[i]);
                  else
                  fprintf(stderr, "err: %d %d\n", compbuff[i], buffer[i]);
                  */
                  
                  //fprintf(stderr, "err: %lu %d %d\n", i, compbuff[i], buffer[i]);
                  ++neq;
               }
            }
            
            if(neq > 0)
            {
               ++fail;
               fprintf(stderr, "reorder_bytepack_renibble_int16_white: %d failed on %d %d %d (%d).\n", neq, ws[w], hs[h], ps[p], ws[w]*hs[h]*ps[p]);
            }
            
            
            free(compbuff);
            xrif_destroy_handle(hand);
         }
      }
   }
   
   
   ck_assert( fail == 0 );
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST

START_TEST (reorder_bytepack_int16_white)
{
   //Verify that xrif reorder-unreorder cycle works for signed 16 bit integers in the bytepack method
   
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   int fail = 0;
   
   for(int q=0;q<100;++q)
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
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               buffer[i] = SHRT_MIN + (((double) rand())/RAND_MAX)*(SHRT_MAX-SHRT_MIN);
            }
   
            int16_t * compbuff = (int16_t *) malloc( hand->width*hand->height*hand->frames*sizeof(int16_t));
            memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(int16_t));
   
            xrif_difference_previous_sint16(hand);
            xrif_reorder_bytepack(hand);

            xrif_unreorder_bytepack(hand);
            xrif_undifference_previous_sint16(hand);
   
            int neq = 0;
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               if(buffer[i] != compbuff[i]) 
               {
                  ++neq;
               }
            }
            
            if(neq > 0)
            {
               ++fail;
               fprintf(stderr, "reorder_bytepack_int16_white: %d failed on %d %d %d (%d).\n", neq, ws[w], hs[h], ps[p], ws[w]*hs[h]*ps[p]);
            }
            
            
            free(compbuff);
            xrif_destroy_handle(hand);
         }
      }
   }
   
   
   ck_assert( fail == 0 );
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST


START_TEST (reorder_bytepack_uint16_white)
{
   //Verify that xrif reorder-unreorder cycle works for unsigned 16 bit integers in the bytepack method
   
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   int ws[] = {5,6,33, 47, 64};
   int hs[] = {5,6,33, 47, 64};
   int ps[] = {2,3,4,5,27,63,64};

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   
   int fail = 0;
   
   for(int q=0;q <100; ++q)
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
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               buffer[i] = (((double) rand())/RAND_MAX)*(USHRT_MAX);
            }
   
            uint16_t * compbuff = (uint16_t *) malloc( hand->width*hand->height*hand->frames*sizeof(uint16_t));
            memcpy(compbuff, buffer, hand->width*hand->height*hand->frames*sizeof(uint16_t));
   
            xrif_difference_previous_sint16(hand);
            xrif_reorder_bytepack(hand);

            xrif_unreorder_bytepack(hand);
            xrif_undifference_previous_sint16(hand);
   
            int neq = 0;
            for( size_t i = 0 ; i < hand->width*hand->height*hand->frames ; ++i ) 
            {
               if(buffer[i] != compbuff[i]) 
               {
                  ++neq;
               }
            }
            
            if(neq > 0)
            {
               ++fail;
               fprintf(stderr, "reorder_bytepack_uint16_white: %d failed on %d %d %d (%d).\n", neq, ws[w], hs[h], ps[p], ws[w]*hs[h]*ps[p]);
            }
            
            
            free(compbuff);
            xrif_destroy_handle(hand);
         }
      }
   }
   
   
   ck_assert( fail == 0 );
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST

Suite * whitenoise_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("White Noise");

    /* Core test case */
    tc_core = tcase_create("16 bit white noise");

    tcase_set_timeout(tc_core, 1e9);
    
    tcase_add_test(tc_core, diff_int16_white);
    tcase_add_test(tc_core, diff_uint16_white);
    tcase_add_test(tc_core, reorder_bytepack_int16_white);
    tcase_add_test(tc_core, reorder_bytepack_uint16_white);
    
    //tcase_add_test(tc_core, reorder_bytepack_renibble_int16_white);
    
    //tcase_add_test(tc_core, reorder_bytepack_int16_white);
    //tcase_add_test(tc_core, reorder_bitpack_int16_white);
    
    //tcase_add_test(tc_core, short_unsigned_white);
    suite_add_tcase(s, tc_core);

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

