/** \file xrif_test_increment.c
  * \brief Test encoding and decoding with incrementing values
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_test_files
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

#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "../src/xrif.h"

#include "incutils.h"

#ifndef XRIF_TEST_TRIALS
   #define XRIF_TEST_TRIALS (2)
#endif

int test_trials;

int ws[] = {2,4,8,21, 33, 47, 64}; //widths of images
int hs[] = {2,4,8,21, 33, 47, 64}; //heights of images
int ps[] = {1,2,4,5,27,63,64}; //planes of the cube

//int ws[] = {120}; //widths of images
//int hs[] = {120}; //heights of images
//int ps[] = {10}; //planes of the cube

/*=====================================================================================
 *
 *                           encoding tests
 * 
 *=====================================================================================*/

/** \test Verify full encode/decode for incrementing int16_t values for previous / bytepack / LZ4
  * \anchor encode_previous_bytepack_lz4_int16_inc
  * 
  * Functions tested:
  *  - \ref xrif_encode(xrif_t)
  *  - \ref xrif_difference(xrif_t)
  *  - \ref xrif_difference_previous(xrif_t)
  *  - \ref xrif_difference_previous_sint16(xrif_t)
  *  - \ref xrif_reorder(xrif_t)
  *  - \ref xrif_reorder_bytepack_sint16(xrif_t)
  *  - \ref xrif_compress(xrif_t)
  *  - \ref xrif_compress_lz4(xrif_t)
  */
START_TEST (encode_previous_bytepack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack / LZ4HC
  * \anchor encode_previous_bytepack_lz4hc_int16_inc
  */
START_TEST (encode_previous_bytepack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;


/** Verify full encode/decode for incrementing int16_t values for previous / bytepack / FastLZ
  * \anchor encode_previous_bytepack_fastlz_int16_inc
  */
START_TEST (encode_previous_bytepack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack / zstd
  * \anchor encode_previous_bytepack_zstd_int16_inc
  */
START_TEST (encode_previous_bytepack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack / zlib
  * \anchor encode_previous_bytepack_zlib_int16_inc
  */
START_TEST (encode_previous_bytepack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack / LZ4
  * \anchor encode_first_bytepack_lz4_int16_inc
  */
START_TEST (encode_first_bytepack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack / LZ4HC
  * \anchor encode_first_bytepack_lz4hc_int16_inc
  */
START_TEST (encode_first_bytepack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack / FastLZ
  * \anchor encode_first_bytepack_fastlz_int16_inc
  */
START_TEST (encode_first_bytepack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack / zstd
  * \anchor encode_first_bytepack_zstd_int16_inc
  */
START_TEST (encode_first_bytepack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack / zlib
  * \anchor encode_first_bytepack_zlib_int16_inc
  */
START_TEST (encode_first_bytepack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;


/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack / LZ4
  * \anchor encode_pixel0_bytepack_lz4_int16_inc
  */
START_TEST (encode_pixel0_bytepack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack / LZ4HC
  * \anchor encode_pixel0_bytepack_lz4hc_int16_inc
  */
START_TEST (encode_pixel0_bytepack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack / FastLZ
  * \anchor encode_pixel0_bytepack_fastlz_int16_inc
  */
START_TEST (encode_pixel0_bytepack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack / zstd
  * \anchor encode_pixel0_bytepack_zstd_int16_inc
  */
START_TEST (encode_pixel0_bytepack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;


/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack / zlib
  * \anchor encode_pixel0_bytepack_zlib_int16_inc
  */
START_TEST (encode_pixel0_bytepack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack / LZ4
  * \anchor encode_pixel1_bytepack_lz4_int16_inc
  */
START_TEST (encode_pixel1_bytepack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack / LZ4HC
  * \anchor encode_pixel1_bytepack_lz4hc_int16_inc
  */
START_TEST (encode_pixel1_bytepack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack / FastLZ
  * \anchor encode_pixel1_bytepack_fastlz_int16_inc
  */
START_TEST (encode_pixel1_bytepack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack / zstd
  * \anchor encode_pixel1_bytepack_zstd_int16_inc
  */
START_TEST (encode_pixel1_bytepack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;


/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack / zlib
  * \anchor encode_pixel1_bytepack_zlib_int16_inc
  */
START_TEST (encode_pixel1_bytepack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack-renibble / LZ4
  * \anchor encode_previous_bytepack_renibble_lz4_int16_inc
  */
START_TEST (encode_previous_bytepack_renibble_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack-renibble / LZ4HC
  * \anchor encode_previous_bytepack_renibble_lz4hc_int16_inc
  */
START_TEST (encode_previous_bytepack_renibble_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack-renibble / FastLZ
  * \anchor encode_previous_bytepack_renibble_fastlz_int16_inc
  */
START_TEST (encode_previous_bytepack_renibble_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack-renibble / zstd
  * \anchor encode_previous_bytepack_renibble_zstd_int16_inc
  */
START_TEST (encode_previous_bytepack_renibble_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bytepack-renibble / zlib
  * \anchor encode_previous_bytepack_renibble_zlib_int16_inc
  */
START_TEST (encode_previous_bytepack_renibble_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack-renibble / LZ4
  * \anchor encode_first_bytepack_renibble_lz4_int16_inc
  */
START_TEST (encode_first_bytepack_renibble_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack-renibble / LZ4HC
  * \anchor encode_first_bytepack_renibble_lz4hc_int16_inc
  */
START_TEST (encode_first_bytepack_renibble_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack-renibble / FastLZ
  * \anchor encode_first_bytepack_renibble_fastlz_int16_inc
  */
START_TEST (encode_first_bytepack_renibble_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack-renibble / zstd
  * \anchor encode_first_bytepack_renibble_zstd_int16_inc
  */
START_TEST (encode_first_bytepack_renibble_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bytepack-renibble / zlib
  * \anchor encode_first_bytepack_renibble_zlib_int16_inc
  */
START_TEST (encode_first_bytepack_renibble_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack-renibble / LZ4
  * \anchor encode_pixel0_bytepack_renibble_lz4_int16_inc
  */
START_TEST (encode_pixel0_bytepack_renibble_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack-renibble / LZ4HC
  * \anchor encode_pixel0_bytepack_renibble_lz4hc_int16_inc
  */
START_TEST (encode_pixel0_bytepack_renibble_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack-renibble / FastLZ
  * \anchor encode_pixel0_bytepack_renibble_fastlz_int16_inc
  */
START_TEST (encode_pixel0_bytepack_renibble_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack-renibble / zstd
  * \anchor encode_pixel0_bytepack_renibble_zstd_int16_inc
  */
START_TEST (encode_pixel0_bytepack_renibble_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bytepack-renibble / zlib
  * \anchor encode_pixel0_bytepack_renibble_zlib_int16_inc
  */
START_TEST (encode_pixel0_bytepack_renibble_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack-renibble / LZ4
  * \anchor encode_pixel1_bytepack_renibble_lz4_int16_inc
  */
START_TEST (encode_pixel1_bytepack_renibble_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack-renibble / LZ4HC
  * \anchor encode_pixel1_bytepack_renibble_lz4hc_int16_inc
  */
START_TEST (encode_pixel1_bytepack_renibble_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack-renibble / FastLZ
  * \anchor encode_pixel1_bytepack_renibble_fastlz_int16_inc
  */
START_TEST (encode_pixel1_bytepack_renibble_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack-renibble / zstd
  * \anchor encode_pixel1_bytepack_renibble_zstd_int16_inc
  */
START_TEST (encode_pixel1_bytepack_renibble_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bytepack-renibble / zlib
  * \anchor encode_pixel1_bytepack_renibble_zlib_int16_inc
  */
START_TEST (encode_pixel1_bytepack_renibble_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BYTEPACK_RENIBBLE)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bitpack / LZ4
  * \anchor encode_previous_bitpack_lz4_int16_inc
  */
START_TEST (encode_previous_bitpack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bitpack / LZ4HC
  * \anchor encode_previous_bitpack_lz4hc_int16_inc
  */
START_TEST (encode_previous_bitpack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bitpack / FastLZ
  * \anchor encode_previous_bitpack_fastlz_int16_inc
  */
START_TEST (encode_previous_bitpack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bitpack / zstd
  * \anchor encode_previous_bitpack_zstd_int16_inc
  */
START_TEST (encode_previous_bitpack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for previous / bitpack / zlib
  * \anchor encode_previous_bitpack_zlib_int16_inc
  */
START_TEST (encode_previous_bitpack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PREVIOUS)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bitpack / LZ4
  * \anchor encode_first_bitpack_lz4_int16_inc
  */
START_TEST (encode_first_bitpack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bitpack / LZ4HC
  * \anchor encode_first_bitpack_lz4hc_int16_inc
  */
START_TEST (encode_first_bitpack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bitpack / FastLZ
  * \anchor encode_first_bitpack_fastlz_int16_inc
  */
START_TEST (encode_first_bitpack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bitpack / zstd
  * \anchor encode_first_bitpack_zstd_int16_inc
  */
START_TEST (encode_first_bitpack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for first / bitpack / zlib
  * \anchor encode_first_bitpack_zlib_int16_inc
  */
START_TEST (encode_first_bitpack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_FIRST)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bitpack / LZ4
  * \anchor encode_pixel0_bitpack_lz4_int16_inc
  */
START_TEST (encode_pixel0_bitpack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bitpack / LZ4HC
  * \anchor encode_pixel0_bitpack_lz4hc_int16_inc
  */
START_TEST (encode_pixel0_bitpack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bitpack / FastLZ
  * \anchor encode_pixel0_bitpack_fastlz_int16_inc
  */
START_TEST (encode_pixel0_bitpack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bitpack / zstd
  * \anchor encode_pixel0_bitpack_zstd_int16_inc
  */
START_TEST (encode_pixel0_bitpack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel0 / bitpack / zlib
  * \anchor encode_pixel0_bitpack_zlib_int16_inc
  */
START_TEST (encode_pixel0_bitpack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL0)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bitpack / LZ4
  * \anchor encode_pixel1_bitpack_lz4_int16_inc
  */
START_TEST (encode_pixel1_bitpack_lz4_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bitpack / LZ4HC
  * \anchor encode_pixel1_bitpack_lz4hc_int16_inc
  */
START_TEST (encode_pixel1_bitpack_lz4hc_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_LZ4HC)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bitpack / FastLZ
  * \anchor encode_pixel1_bitpack_fastlz_int16_inc
  */
START_TEST (encode_pixel1_bitpack_fastlz_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_FASTLZ)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bitpack / zstd
  * \anchor encode_pixel1_bitpack_zstd_int16_inc
  */
START_TEST (encode_pixel1_bitpack_zstd_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZSTD)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

/** Verify full encode/decode for incrementing int16_t values for pixel1 / bitpack / zlib
  * \anchor encode_pixel1_bitpack_zlib_int16_inc
  */
START_TEST (encode_pixel1_bitpack_zlib_int16_inc)
{
   #define XRIF_TESTLOOP_TYPECODE (XRIF_TYPECODE_INT16)
   #define XRIF_TESTLOOP_DIFFERENCE (XRIF_DIFFERENCE_PIXEL1)
   #define XRIF_TESTLOOP_REORDER (XRIF_REORDER_BITPACK)
   #define XRIF_TESTLOOP_COMPRESS (XRIF_COMPRESS_ZLIB)
   #define XRIF_TESTLOOP_FILL 0
   #define XRIF_TESTLOOP_ENCODE xrif_encode
   #define XRIF_TESTLOOP_DECODE xrif_decode
   
   #include "testloop.c"
}
END_TEST;

Suite * increment_suite(void)
{
      Suite *s;
      TCase *tc_core;

      s = suite_create("Incrementing");

      /* Core test case */
      tc_core = tcase_create("16 bit incrementing pixels");

      tcase_set_timeout(tc_core, 1e9);
    
    
      // previous / bytepack
      tcase_add_test(tc_core, encode_previous_bytepack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_zlib_int16_inc);

      // first / bytepack
      tcase_add_test(tc_core, encode_first_bytepack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_zlib_int16_inc);

      // pixel0 / bytepack
      tcase_add_test(tc_core, encode_pixel0_bytepack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_zlib_int16_inc);

      // pixel1 / bytepack
      tcase_add_test(tc_core, encode_pixel1_bytepack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_zlib_int16_inc);

      // previous / bytepack-renibble
      tcase_add_test(tc_core, encode_previous_bytepack_renibble_lz4_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_renibble_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_renibble_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_renibble_zstd_int16_inc);
      tcase_add_test(tc_core, encode_previous_bytepack_renibble_zlib_int16_inc);

      // first / bytepack-renibble
      tcase_add_test(tc_core, encode_first_bytepack_renibble_lz4_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_renibble_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_renibble_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_renibble_zstd_int16_inc);
      tcase_add_test(tc_core, encode_first_bytepack_renibble_zlib_int16_inc);

      // pixel0 / bytepack-renibble
      tcase_add_test(tc_core, encode_pixel0_bytepack_renibble_lz4_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_renibble_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_renibble_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_renibble_zstd_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bytepack_renibble_zlib_int16_inc);

      // pixel1 / bytepack-renibble
      tcase_add_test(tc_core, encode_pixel1_bytepack_renibble_lz4_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_renibble_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_renibble_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_renibble_zstd_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bytepack_renibble_zlib_int16_inc);

      // previous / bitpack
      tcase_add_test(tc_core, encode_previous_bitpack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_previous_bitpack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_previous_bitpack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_previous_bitpack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_previous_bitpack_zlib_int16_inc);

      // first / bitpack
      tcase_add_test(tc_core, encode_first_bitpack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_first_bitpack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_first_bitpack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_first_bitpack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_first_bitpack_zlib_int16_inc);

      // pixel0 / bitpack
      tcase_add_test(tc_core, encode_pixel0_bitpack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bitpack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bitpack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bitpack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_pixel0_bitpack_zlib_int16_inc);

      // pixel1 / bitpack
      tcase_add_test(tc_core, encode_pixel1_bitpack_lz4_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bitpack_lz4hc_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bitpack_fastlz_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bitpack_zstd_int16_inc);
      tcase_add_test(tc_core, encode_pixel1_bitpack_zlib_int16_inc);

      suite_add_tcase(s, tc_core);

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

   s = increment_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   number_failed = srunner_ntests_failed(sr);
   srunner_free(sr);
   
   return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
   
}

