/** \file xrif_test_difference_pixel_whitenoise.c
  * \brief Test the pixel differencing method with white noise.
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_test_files
  */

/* This file is part of the xrif library.

Copyright (c) 2019, 2020, 2021 The Arizona Board of Regents on behalf of The
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
   #define XRIF_TEST_TRIALS (2)
#endif

int test_trials;

/************************************************************/
/* Fuzz testing differencing with the pixel method
/************************************************************/


int ws[] = {2,4,8,21, 33, 47, 64}; //widths of images
int hs[] = {2,4,8,21, 33, 47, 64}; //heights of images
int ps[] = {1,2,4,5,27,63,64}; //planes of the cube
 

 
/** Verify pixel differencing for int16_t
  * Verify that xrif difference/un-difference cycle using the first pixel of each works with white noise For int16_t.
  * \anchor diff_pixel_int16_white
  */
START_TEST (diff_pixel_int16_white)
{
   fprintf(stderr, "Testing pixel differencing for signed 16-bit white noise.\n");
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 1
   #define XRIF_TESTLOOP_ENCODE xrif_difference_previous
   #define XRIF_TESTLOOP_DECODE xrif_undifference_previous
   #define XRIF_TESTLOOP_NOPERF
   
   #include "testloop.c"
}
END_TEST;

/** Verify pixel differencing for uint16_t
  * Verify that xrif difference/un-difference cycle using the first pixel of each works with white noise For uint16_t.
  * \anchor diff_pixel_uint16_white
  */
START_TEST (diff_pixel_uint16_white)
{
   fprintf(stderr, "Testing pixel differencing for unsigned 16-bit white noise.\n");
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_UINT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 1
   #define XRIF_TESTLOOP_ENCODE xrif_difference_previous
   #define XRIF_TESTLOOP_DECODE xrif_undifference_previous
   #define XRIF_TESTLOOP_NOPERF
   
   #include "testloop.c"
}
END_TEST;

/** Verify pixel differencing for int32_t
  * Verify that xrif difference/un-difference cycle using the first pixel of each works with white noise For int32_t.
  * \anchor diff_pixel_int32_white
  */
START_TEST (diff_pixel_int32_white)
{
   fprintf(stderr, "Testing pixel differencing for signed 32-bit white noise.\n");
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT32)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 1
   #define XRIF_TESTLOOP_ENCODE xrif_difference_previous
   #define XRIF_TESTLOOP_DECODE xrif_undifference_previous
   #define XRIF_TESTLOOP_NOPERF
   
   #include "testloop.c" 
}
END_TEST;

/** Verify pixel differencing for uint32_t
  * Verify that xrif difference/un-difference cycle using the first pixel of each works with white noise For uint32_t.
  * \anchor diff_pixel_uint32_white
  */
START_TEST (diff_pixel_uint32_white)
{
   fprintf(stderr, "Testing pixel differencing for unsigned 32-bit white noise.\n");
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_UINT32)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 1
   #define XRIF_TESTLOOP_ENCODE xrif_difference_previous
   #define XRIF_TESTLOOP_DECODE xrif_undifference_previous
   #define XRIF_TESTLOOP_NOPERF
   
   #include "testloop.c" 
}
END_TEST;

/** Verify pixel differencing for int64_t
  * Verify that xrif difference/un-difference cycle using the first pixel of each works with white noise For int64_t.
  * \anchor diff_pixel_int64_white
  */
START_TEST (diff_pixel_int64_white)
{
   fprintf(stderr, "Testing pixel differencing for signed 64-bit white noise.\n");
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT64)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 1
   #define XRIF_TESTLOOP_ENCODE xrif_difference_previous
   #define XRIF_TESTLOOP_DECODE xrif_undifference_previous
   #define XRIF_TESTLOOP_NOPERF
   
   #include "testloop.c" 
}
END_TEST;

/** Verify pixel differencing for uint64_t
  * Verify that xrif difference/un-difference cycle using the first pixel of each works with white noise For uint64_t.
  * \anchor diff_pixel_uint64_white
  */
START_TEST (diff_pixel_uint64_white)
{
   fprintf(stderr, "Testing pixel differencing for unsigned 64-bit white noise.\n");
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_UINT64)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 1
   #define XRIF_TESTLOOP_ENCODE xrif_difference_previous
   #define XRIF_TESTLOOP_DECODE xrif_undifference_previous
   #define XRIF_TESTLOOP_NOPERF
   
   #include "testloop.c" 
}
END_TEST;

Suite * whitenoise_suite(void)
{
    Suite *s;
    TCase *tc_core16, *tc_core32, *tc_core64;

    s = suite_create("White Noise - Difference Pixel");

    /* 16-bit Core test case */
    tc_core16 = tcase_create("16 bit white noise");

    tcase_set_timeout(tc_core16, 1e9);
    
    tcase_add_test(tc_core16, diff_pixel_int16_white);
    tcase_add_test(tc_core16, diff_pixel_uint16_white);
    
    suite_add_tcase(s, tc_core16);
    
    /* 32-bit Core test case */
    tc_core32 = tcase_create("32 bit white noise");

    tcase_set_timeout(tc_core32, 1e9);
    
    tcase_add_test(tc_core32, diff_pixel_int32_white);
    tcase_add_test(tc_core32, diff_pixel_uint32_white);
    
    suite_add_tcase(s, tc_core32);

    /* 64-bit Core test case */
    tc_core64 = tcase_create("64 bit white noise");

    tcase_set_timeout(tc_core64, 1e9);
    
    tcase_add_test(tc_core64, diff_pixel_int64_white);
    tcase_add_test(tc_core64, diff_pixel_uint64_white);
    
    suite_add_tcase(s, tc_core64);
    
    return s;
}

int main( int argc,
          char ** argv
        )
{
   
   extern int test_trials;
   
   test_trials = XRIF_TEST_TRIALS;
   
   if(argc == 2)
   {
      test_trials = atoi(argv[1]);
   }
   
   fprintf(stderr, "running %d trials per format\n", test_trials);
   
   int number_failed;
   Suite *s;
   SRunner *sr;

   // Intialize the random number sequence
   srand((unsigned) time(NULL));
   

   
   s = whitenoise_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   number_failed = srunner_ntests_failed(sr);
   srunner_free(sr);
   
   return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
   
}

