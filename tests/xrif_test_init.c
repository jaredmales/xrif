
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
#include <malloc.h>

#include "../src/xrif.h"

extern void *__libc_malloc(size_t size);

int malloc_hook_active = 0;

void* my_malloc_hook (size_t size, void *caller)
{
  void *result;

  // deactivate hooks for logging
  malloc_hook_active = 0;

  result = malloc(size);

  fprintf(stderr, "malloc\n");

  // reactivate hooks
  malloc_hook_active = 1;

  return result;
}

void* malloc (size_t size)
{
  void *caller = __builtin_return_address(0);
  if (malloc_hook_active)
    return my_malloc_hook(size, caller);
  return __libc_malloc(size);
}


START_TEST (initialize_handle_noerror)
{
   //Verify that all fields are initialized to their defaults.
   
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert_int_eq( hand.width, 0);
   ck_assert_int_eq( hand.height, 0);
   ck_assert_int_eq( hand.depth, 0);
   ck_assert_int_eq( hand.frames, 0);
   ck_assert_int_eq( hand.type_code, 0);
   ck_assert_int_eq( hand.data_size, 0);
   ck_assert_int_eq( hand.compressed_size, 0);
   ck_assert_int_eq( hand.difference_method, XRIF_DIFFERENCE_DEFAULT);
   ck_assert_int_eq( hand.reorder_method, XRIF_REORDER_DEFAULT);
   ck_assert_int_eq( hand.compress_method, XRIF_COMPRESS_DEFAULT);
   ck_assert_int_eq( hand.lz4_acceleration, 1);
   ck_assert_int_eq( hand.omp_parallel, 0);
   ck_assert_int_eq( hand.omp_numthreads, 1);
   ck_assert_int_eq( hand.compress_on_raw, 1);
   ck_assert_int_eq( hand.own_raw, 0);
   ck_assert( hand.raw_buffer == NULL );
   ck_assert_int_eq( hand.raw_buffer_size, 0);
   ck_assert_int_eq( hand.own_reordered, 0);
   ck_assert( hand.reordered_buffer == NULL );
   ck_assert_int_eq( hand.reordered_buffer_size, 0);
   ck_assert_int_eq( hand.own_compressed, 0);
   ck_assert( hand.compressed_buffer == NULL );
   ck_assert_int_eq( hand.compressed_buffer_size, 0);
   
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST

START_TEST (initialize_handle_nullptr)
{
   //Verify that all fields are initialized to their defaults.
   
   xrif_handle * hand = NULL;
   
   xrif_error_t rv = xrif_initialize_handle(hand);

   ck_assert( rv == XRIF_ERROR_NULLPTR );
}
END_TEST

START_TEST (new_delete_noerror)
{
   //Verify that all fields are initialized to their defaults.
   
   xrif_t hand = 0;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert_int_eq( hand->width, 0);
   ck_assert_int_eq( hand->height, 0);
   ck_assert_int_eq( hand->depth, 0);
   ck_assert_int_eq( hand->frames, 0);
   ck_assert_int_eq( hand->type_code, 0);
   ck_assert_int_eq( hand->data_size, 0);
   ck_assert_int_eq( hand->compressed_size, 0);
   ck_assert_int_eq( hand->difference_method, XRIF_DIFFERENCE_DEFAULT);
   ck_assert_int_eq( hand->reorder_method, XRIF_REORDER_DEFAULT);
   ck_assert_int_eq( hand->compress_method, XRIF_COMPRESS_DEFAULT);
   ck_assert_int_eq( hand->lz4_acceleration, 1);
   ck_assert_int_eq( hand->omp_parallel, 0);
   ck_assert_int_eq( hand->omp_numthreads, 1);
   ck_assert_int_eq( hand->compress_on_raw, 1);
   ck_assert_int_eq( hand->own_raw, 0);
   ck_assert( hand->raw_buffer == NULL );
   ck_assert_int_eq( hand->raw_buffer_size, 0);
   ck_assert_int_eq( hand->own_reordered, 0);
   ck_assert( hand->reordered_buffer == NULL );
   ck_assert_int_eq( hand->reordered_buffer_size, 0);
   ck_assert_int_eq( hand->own_compressed, 0);
   ck_assert( hand->compressed_buffer == NULL );
   ck_assert_int_eq( hand->compressed_buffer_size, 0);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST


START_TEST (setup_noerror)
{
   //Verfiy that the setup function sets only the expected members
   
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 1024,64,32,1000, XRIF_TYPECODE_INT16);
   
   ck_assert_int_eq( hand.width, 1024);
   ck_assert_int_eq( hand.height, 64);
   ck_assert_int_eq( hand.depth, 32);
   ck_assert_int_eq( hand.frames, 1000);
   ck_assert_int_eq( hand.type_code, XRIF_TYPECODE_INT16);
   ck_assert_int_eq( hand.data_size, sizeof(int16_t));
   
   //And we check that everything else is unaltered
   ck_assert_int_eq( hand.compressed_size, 0);
   ck_assert_int_eq( hand.difference_method, XRIF_DIFFERENCE_DEFAULT);
   ck_assert_int_eq( hand.reorder_method, XRIF_REORDER_DEFAULT);
   ck_assert_int_eq( hand.compress_method, XRIF_COMPRESS_DEFAULT);
   ck_assert_int_eq( hand.lz4_acceleration, 1);
   ck_assert_int_eq( hand.omp_parallel, 0);
   ck_assert_int_eq( hand.omp_numthreads, 1);
   ck_assert_int_eq( hand.compress_on_raw, 1);
   ck_assert_int_eq( hand.own_raw, 0);
   ck_assert( hand.raw_buffer == NULL );
   ck_assert_int_eq( hand.raw_buffer_size, 0);
   ck_assert_int_eq( hand.own_reordered, 0);
   ck_assert( hand.reordered_buffer == NULL );
   ck_assert_int_eq( hand.reordered_buffer_size, 0);
   ck_assert_int_eq( hand.own_compressed, 0);
   ck_assert( hand.compressed_buffer == NULL );
   ck_assert_int_eq( hand.compressed_buffer_size, 0);
   
   ck_assert( rv == XRIF_NOERROR );
}
END_TEST

START_TEST (setup_nullptr)
{
   //Verfiy that the setup function returns an error on a null ptr
   
   xrif_error_t rv = xrif_initialize_handle(NULL);
   
   ck_assert( rv == XRIF_ERROR_NULLPTR );
   
}
END_TEST

START_TEST (set_raw_noerrors)
{
   //Verfiy that the raw buffer setup works properly
   
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,3,120, XRIF_TYPECODE_INT16);
   
   ck_assert( rv == XRIF_NOERROR );
      
   //First do this with compress_on_raw == false
   hand.compress_on_raw = 0;
   int16_t buff;
   
   rv = xrif_set_raw( &hand, &buff, 120*120*3*120*sizeof(int16_t));
   
   ck_assert( hand.own_raw == 0 );
   ck_assert( hand.raw_buffer == (char *) &buff );
   ck_assert( hand.raw_buffer_size = 120*120*3*120*sizeof(int16_t) );

   ck_assert( rv == XRIF_NOERROR );
   
   //And then do this with compress_on_raw == true
   hand.compress_on_raw = 1;
   
   rv = xrif_set_raw( &hand, &buff, LZ4_compressBound( 120*120*3*120*sizeof(int16_t) ));
   
   ck_assert( hand.own_raw == 0 );
   ck_assert( hand.raw_buffer == (char *) &buff );
   ck_assert( hand.raw_buffer_size = LZ4_compressBound( 120*120*3*120*sizeof(int16_t) ) );
   
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST

START_TEST (set_raw_errors)
{
   //Verfiy that the raw buffer returns expected errors.
   
   xrif_error_t rv = xrif_set_raw(NULL,0,0);
   ck_assert( rv == XRIF_ERROR_NULLPTR );
   
   xrif_handle hand;
   
   rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,3,120, XRIF_TYPECODE_INT16);
   
   ck_assert( rv == XRIF_NOERROR );
 
   // Test case where buffer is not null, but size is 0
   int16_t * buff = (uint16_t *) 10;
   
   rv = xrif_set_raw( &hand, buff, 0); 
   
   ck_assert( rv == XRIF_ERROR_INVALID_SIZE );
   
   buff = NULL;
   
   rv = xrif_set_raw( &hand, buff, 10); 
   
   ck_assert( rv == XRIF_ERROR_INVALID_SIZE );
   
   //--Now test errors on wrong sizes
   //First do this with compress_on_raw == false
   hand.compress_on_raw = 0;
   
   rv = xrif_set_raw( &hand, buff, 0.5*120*120*3*120*sizeof(int16_t));
   
   ck_assert( hand.own_raw == 0 );
   ck_assert( hand.raw_buffer == (char *) buff );
   ck_assert( hand.raw_buffer_size = 0.5*120*120*3*120*sizeof(int16_t) );
   
   ck_assert( rv == XRIF_ERROR_INSUFFICIENT_SIZE );
   
   //And then do this with compress_on_raw == true
   hand.compress_on_raw = 1;
   
   rv = xrif_set_raw( &hand, buff,  0.5*120*120*3*120*sizeof(int16_t) );
   
   ck_assert( hand.own_raw == 0 );
   ck_assert( hand.raw_buffer == (char *) buff );
   ck_assert( hand.raw_buffer_size = 0.5*120*120*3*120*sizeof(int16_t)  );
   
   ck_assert( rv == XRIF_ERROR_INSUFFICIENT_SIZE );
}
END_TEST



START_TEST (set_reordered_noerrors)
{
   //Verfiy that the reordered buffer setup works properly
   
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,3,120, XRIF_TYPECODE_INT16);
   
   ck_assert( rv == XRIF_NOERROR );
      
   int16_t buff;
   
   rv = xrif_set_reordered( &hand, &buff, 120*120*3*120*sizeof(int16_t));
   
   ck_assert( hand.own_reordered == 0 );
   ck_assert( hand.reordered_buffer == (char *) &buff );
   ck_assert( hand.reordered_buffer_size = 120*120*3*120*sizeof(int16_t) );
   
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST

START_TEST (set_reordered_errors)
{
   //Verfiy that the raw buffer returns expected errors.
   
   xrif_error_t rv = xrif_set_reordered(NULL,0,0);
   ck_assert( rv == XRIF_ERROR_NULLPTR );
   
   xrif_handle hand;
   
   rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,3,120, XRIF_TYPECODE_INT16);
   
   ck_assert( rv == XRIF_NOERROR );
 
   // Test case where buffer is not null, but size is 0
   int16_t * buff = (uint16_t *) 10;
   
   rv = xrif_set_reordered( &hand, buff, 0); 
   
   ck_assert( rv == XRIF_ERROR_INVALID_SIZE );
   
   buff = NULL;
   
   rv = xrif_set_reordered( &hand, buff, 10); 
   
   ck_assert( rv == XRIF_ERROR_INVALID_SIZE );
}
END_TEST

START_TEST (set_compressed_noerrors)
{
   //Verfiy that the compress buffer setup works properly
   
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,3,120, XRIF_TYPECODE_INT16);
   
   ck_assert( rv == XRIF_NOERROR );
      
   int16_t buff;
   size_t lz4sz = LZ4_compressBound(120*120*3*120*sizeof(int16_t));
   
   rv = xrif_set_compressed( &hand, &buff, lz4sz);
   
   ck_assert( hand.own_compressed == 0 );
   ck_assert( hand.compressed_buffer == (char *) &buff );
   ck_assert( hand.compressed_buffer_size = lz4sz);
   
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST

START_TEST (set_compressed_errors)
{
   //Verfiy that the raw buffer returns expected errors.
   
   xrif_error_t rv = xrif_set_compressed(NULL,0,0);
   ck_assert( rv == XRIF_ERROR_NULLPTR );
   
   xrif_handle hand;
   
   rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,3,120, XRIF_TYPECODE_INT16);
   
   ck_assert( rv == XRIF_NOERROR );
 
   // Test case where buffer is not null, but size is 0
   int16_t * buff = (uint16_t *) 10;
   
   rv = xrif_set_compressed( &hand, buff, 0); 
   
   ck_assert( rv == XRIF_ERROR_INVALID_SIZE );
   
   buff = NULL;
   
   rv = xrif_set_compressed( &hand, buff, 10); 
   
   ck_assert( rv == XRIF_ERROR_INVALID_SIZE );
   
   //Check that not matching LZ4_compressBound returns error
   buff = (uint16_t *) 10;
   rv = xrif_set_compressed( &hand, buff, 1024); 
   ck_assert( rv == XRIF_ERROR_INSUFFICIENT_SIZE );
}
END_TEST


START_TEST (allocate_raw_noerrors)
{
   //Verfiy that the compress buffer setup works properly
   
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,3,120, XRIF_TYPECODE_INT16);
   
   ck_assert( rv == XRIF_NOERROR );
      
   rv = xrif_allocate_raw(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
}
END_TEST

//-------------------------------------------------------------------

START_TEST (header_write)
{
   //This test verifies that header fields are correctly populated
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,120,1,1000, XRIF_TYPECODE_INT16);
      
   ck_assert( rv == XRIF_NOERROR );
   
   
   char header[XRIF_HEADER_SIZE];
   
   rv = xrif_write_header( header, &hand );

   ck_assert( rv == XRIF_NOERROR );
   
   ck_assert( header[0] == 'x' );
   ck_assert( header[1] == 'r' );
   ck_assert( header[2] == 'i' );
   ck_assert( header[3] == 'f' );
 
   ck_assert( *((uint32_t *) &header[4]) == XRIF_VERSION);   
   ck_assert( *((uint32_t *) &header[8]) == XRIF_HEADER_SIZE); 
   ck_assert( *((uint32_t *) &header[12]) == hand.width);
   ck_assert( *((uint32_t *) &header[16]) == hand.height);
   ck_assert( *((uint32_t *) &header[20]) == hand.depth);
   ck_assert( *((uint32_t *) &header[24]) == hand.frames);
   ck_assert( *((uint16_t *) &header[28]) == hand.type_code);
   ck_assert( *((uint16_t *) &header[30]) == hand.difference_method);
   ck_assert( *((uint16_t *) &header[32]) == hand.reorder_method);
   ck_assert( *((uint16_t *) &header[34]) == hand.compress_method);
   ck_assert( *((uint16_t *) &header[36]) == hand.lz4_acceleration);
   ck_assert( *((uint16_t *) &header[38]) == 0);
   ck_assert( *((uint64_t *) &header[40]) == 0);
}
END_TEST

START_TEST (header_read)
{
   //This test writes a setup handle to a header
   //Then reads it to a new handle, verifying that members are set properly.
   
   xrif_handle hand;
   
   xrif_error_t rv = xrif_initialize_handle(&hand);
   
   ck_assert( rv == XRIF_NOERROR );
   
   rv = xrif_set_size(&hand, 120,240,2,1000, XRIF_TYPECODE_INT16);
      
   ck_assert( rv == XRIF_NOERROR );
   
   hand.lz4_acceleration = 10; //Change this from init value
   
   char header[XRIF_HEADER_SIZE];
   
   rv = xrif_write_header( header, &hand );

   ck_assert( rv == XRIF_NOERROR );
   
   //New handle to populate with read from header
   xrif_handle hand2;
   
   rv = xrif_initialize_handle(&hand2);
   
   ck_assert( rv == XRIF_NOERROR );
   
   uint32_t header_size;
   rv = xrif_read_header( &hand2, &header_size, header );

   ck_assert( rv == XRIF_NOERROR );

   ck_assert_int_eq( header_size, XRIF_HEADER_SIZE);
   ck_assert_int_eq( hand2.width, 120);
   ck_assert_int_eq( hand2.height, 240);
   ck_assert_int_eq( hand2.depth, 2);
   ck_assert_int_eq( hand2.frames, 1000);
   ck_assert_int_eq( hand2.type_code, XRIF_TYPECODE_INT16);
   ck_assert_int_eq( hand2.data_size, sizeof(int16_t));
   ck_assert_int_eq( hand2.lz4_acceleration, 10);
   
   //And we check that everything else is unaltered
   ck_assert_int_eq( hand2.compressed_size, 0);
   ck_assert_int_eq( hand2.difference_method, XRIF_DIFFERENCE_DEFAULT);
   ck_assert_int_eq( hand2.reorder_method, XRIF_REORDER_DEFAULT);
   ck_assert_int_eq( hand2.compress_method, XRIF_COMPRESS_DEFAULT);
   ck_assert_int_eq( hand2.omp_parallel, 0);
   ck_assert_int_eq( hand2.omp_numthreads, 1);
   ck_assert_int_eq( hand2.compress_on_raw, 1);
   ck_assert_int_eq( hand2.own_raw, 0);
   ck_assert( hand2.raw_buffer == NULL );
   ck_assert_int_eq( hand2.raw_buffer_size, 0);
   ck_assert_int_eq( hand2.own_reordered, 0);
   ck_assert( hand2.reordered_buffer == NULL );
   ck_assert_int_eq( hand2.reordered_buffer_size, 0);
   ck_assert_int_eq( hand2.own_compressed, 0);
   ck_assert( hand2.compressed_buffer == NULL );
   ck_assert_int_eq( hand2.compressed_buffer_size, 0);

}

END_TEST

Suite * initandalloc_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Initialize and Allocate");

    /* Core test case */
    tc_core = tcase_create("Initial Setups");

    tcase_add_test(tc_core, initialize_handle_noerror);
    tcase_add_test(tc_core, initialize_handle_nullptr);
    tcase_add_test(tc_core, new_delete_noerror);
    tcase_add_test(tc_core, setup_noerror );
    tcase_add_test(tc_core, setup_nullptr );
    tcase_add_test(tc_core, allocate_raw_noerrors);
    tcase_add_test(tc_core, set_raw_noerrors);
    tcase_add_test(tc_core, set_raw_noerrors);
    tcase_add_test(tc_core, set_reordered_noerrors);
    tcase_add_test(tc_core, set_reordered_errors);
    tcase_add_test(tc_core, set_compressed_noerrors);
    tcase_add_test(tc_core, set_compressed_errors);
    suite_add_tcase(s, tc_core);

    return s;
}

Suite * headerformat_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Header Formatting");

    /* Core test case */
    tc_core = tcase_create("Write and Read No Errors");

    tcase_add_test(tc_core, header_write );
    tcase_add_test(tc_core, header_read );
    suite_add_tcase(s, tc_core);

    return s;
}

int main()
{
   int number_failed;
   Suite *s;
   SRunner *sr;

   s = initandalloc_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   number_failed = srunner_ntests_failed(sr);
   srunner_free(sr);
   
   s = headerformat_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   number_failed = srunner_ntests_failed(sr);
   srunner_free(sr);
      
   return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
   
}

