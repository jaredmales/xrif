
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "../src/xrif.h"


// Verify that xrif difference/un-difference cycle using pixels works with white noise
// For int16_t
START_TEST (ascii_pixel_none_lz4_xrif_h)
{
   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
 
   char fname[] = "../src/xrif.h";

   FILE * fp = fopen(fname ,"rb");
   //get size.
   struct stat stbuf;
 
   if ((fstat(fileno(fp), &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) 
   {
      ck_assert( 0 );
   }
  
   size_t sz = stbuf.st_size;
               
   rv = xrif_configure(hand, XRIF_DIFFERENCE_PIXEL, XRIF_REORDER_NONE, XRIF_COMPRESS_LZ4);
   ck_assert( rv == XRIF_NOERROR );
               
   rv = xrif_set_size(hand, sz, 1, 1, 1, XRIF_TYPECODE_INT8);
   ck_assert( rv == XRIF_NOERROR );
      
   rv = xrif_allocate(hand);
   ck_assert( rv == XRIF_NOERROR );
               
   rv = fread(hand->m_raw_buffer, 1, sz, fp);
   ck_assert( rv == sz );
              
   int8_t * compbuff = (int8_t *) malloc( hand->m_width*hand->m_height*hand->m_frames*sizeof(int8_t));
   memcpy(compbuff, hand->m_raw_buffer, hand->m_width*hand->m_height*hand->m_frames*sizeof(int8_t));
            
   xrif_encode(hand);
   xrif_decode(hand);
               
   int neq = 0;
   for( size_t i = 0 ; i < hand->m_width*hand->m_height*hand->m_frames ; ++i ) 
   {
      if(hand->m_raw_buffer[i] != compbuff[i]) 
      {
         ++neq;
      }
   }
   ck_assert( neq == 0 );
   
   free(compbuff);
   double dt_difference = xrif_difference_time(hand);
   double dt_reorder = xrif_reorder_time(hand);
   double dt_compress = xrif_compress_time(hand);
   double dt_encode = xrif_encode_time(hand);
   double dt_ratio = xrif_compression_ratio(hand);
   double dt_decompress = xrif_decompress_time(hand);
   double dt_unreorder = xrif_unreorder_time(hand);
   double dt_undifference = xrif_undifference_time(hand);
   double dt_decode = xrif_decode_time(hand);
               
   printf("   ratio:           %0.2f %%\n", dt_ratio/1.0*100);
   printf("   encode:          %0.2f MB/s\n", sz/(dt_encode/1.0)/1048576);
   printf("      difference:   %0.2f MB/s\n", sz/(dt_difference/1.0)/1048576);
   printf("      reorder:      %0.2f MB/s\n", sz/(dt_reorder/1.0)/1048576);
   printf("      compress:     %0.2f MB/s\n", sz/(dt_compress/1.0)/1048576);
   printf("   decode:          %0.2f MB/s\n", sz/(dt_decode/1.0)/1048576);            
   printf("      decompress:   %0.2f MB/s\n", sz/(dt_decompress/1.0)/1048576);
   printf("      unreorder:    %0.2f MB/s\n", sz/(dt_unreorder/1.0)/1048576);
   printf("      undifference: %0.2f MB/s\n", sz/(dt_undifference/1.0)/1048576);
            
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST









Suite * whitenoise_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Compressing ASCII");

    /* Core test case */
    tc_core = tcase_create("compressing xrif.h");

    tcase_set_timeout(tc_core, 1e9);
    
    tcase_add_test(tc_core, ascii_pixel_none_lz4_xrif_h);
    
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

