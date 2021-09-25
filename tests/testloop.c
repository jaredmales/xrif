#if XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT16
   #define XRIF_TESTLOOP_TYPE int16_t
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT16
   #define XRIF_TESTLOOP_TYPE uint16_t
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT32
   #define XRIF_TESTLOOP_TYPE int32_t
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT32
   #define XRIF_TESTLOOP_TYPE uint32_t
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT64
   #define XRIF_TESTLOOP_TYPE int64_t
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT64
   #define XRIF_TESTLOOP_TYPE uint64_t
#endif

#if XRIF_TESTLOOP_FILL == 0

#if XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT16
   #define XRIF_TESTLOOP_FILL_FUNC fill_int16_inc
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT16
   #define XRIF_TESTLOOP_FILL_FUNC fill_uint16_inc
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT32
   #define XRIF_TESTLOOP_FILL_FUNC fill_int32_inc
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT32
   #define XRIF_TESTLOOP_FILL_FUNC fill_uint32_inc
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT64
   #define XRIF_TESTLOOP_FILL_FUNC fill_int64_inc
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT64
   #define XRIF_TESTLOOP_FILL_FUNC fill_uint64_inc
#endif

#elif XRIF_TESTLOOP_FILL == 1

#if XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT16
   #define XRIF_TESTLOOP_FILL_FUNC fill_int16_white
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT16
   #define XRIF_TESTLOOP_FILL_FUNC fill_uint16_white
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT32
   #define XRIF_TESTLOOP_FILL_FUNC fill_int32_white
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT32
   #define XRIF_TESTLOOP_FILL_FUNC fill_uint32_white
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_INT64
   #define XRIF_TESTLOOP_FILL_FUNC fill_int64_white
#elif XRIF_TESTLOOP_TYPECODE == XRIF_TYPECODE_UINT64
   #define XRIF_TESTLOOP_FILL_FUNC fill_uint64_white
#endif

#endif

#if XRIF_TESTLOOP_DIFFERENCE == XRIF_DIFFERENCE_NONE
   #define XRIF_TESTLOOP_DIFF_STR "none"
#elif XRIF_TESTLOOP_DIFFERENCE == XRIF_DIFFERENCE_PREVIOUS
   #define XRIF_TESTLOOP_DIFF_STR "previous"
#elif XRIF_TESTLOOP_DIFFERENCE == XRIF_DIFFERENCE_FIRST
   #define XRIF_TESTLOOP_DIFF_STR "first"
#elif XRIF_TESTLOOP_DIFFERENCE == XRIF_DIFFERENCE_PIXEL
   #define XRIF_TESTLOOP_DIFF_STR "pixel"
#endif

#if XRIF_TESTLOOP_REORDER == XRIF_REORDER_NONE
   #define XRIF_TESTLOOP_REORD_STR "none"
#elif XRIF_TESTLOOP_REORDER == XRIF_REORDER_BYTEPACK
   #define XRIF_TESTLOOP_REORD_STR "bytepack"
#elif XRIF_TESTLOOP_REORDER == XRIF_REORDER_BYTEPACK_RENIBBLE
   #define XRIF_TESTLOOP_REORD_STR "bytepack-renibble"
#elif XRIF_TESTLOOP_REORDER == XRIF_REORDER_BITPACK
   #define XRIF_TESTLOOP_REORD_STR "bitpack"
#endif

#if XRIF_TESTLOOP_COMPRESS == XRIF_COMPRESS_NONE
   #define XRIF_TESTLOOP_COMP_STR "none"
#elif XRIF_TESTLOOP_COMPRESS == XRIF_COMPRESS_LZ4
   #define XRIF_TESTLOOP_COMP_STR "lz4"
#elif XRIF_TESTLOOP_COMPRESS == XRIF_COMPRESS_LZ4HC
   #define XRIF_TESTLOOP_COMP_STR "lz4hc"
#elif XRIF_TESTLOOP_COMPRESS == XRIF_COMPRESS_FASTLZ
   #define XRIF_TESTLOOP_COMP_STR "fastlz"
#elif XRIF_TESTLOOP_COMPRESS == XRIF_COMPRESS_ZSTD
   #define XRIF_TESTLOOP_COMP_STR "zstd"
#elif XRIF_TESTLOOP_COMPRESS == XRIF_COMPRESS_ZLIB
   #define XRIF_TESTLOOP_COMP_STR "zlib"
#endif

   xrif_t hand = NULL;
   
   xrif_error_t rv = xrif_new(&hand);
   
   ck_assert( hand != NULL);
   ck_assert( rv == XRIF_NOERROR );
    
   int fail = 0;
   
   for(int w =0; w < sizeof(ws)/sizeof(ws[0]); ++w)
   {
      for(int h=0; h < sizeof(hs)/sizeof(hs[0]); ++h)
      {
         //if(ws[w] == hs[h]) continue;
         for(int p=0; p< sizeof(ps)/sizeof(ps[0]); ++p)
         {
            double dt_difference=0;
            double dt_reorder = 0;
            double dt_compress = 0;
            double dt_encode = 0;
            double dt_ratio = 0;
            double dt_decompress = 0;
            double dt_unreorder = 0;
            double dt_undifference = 0;
            double dt_decode = 0;
            
            for(int q=0; q < test_trials; ++q)
            {

               rv = xrif_set_size(hand, ws[w], hs[h], 1, ps[p], XRIF_TESTLOOP_TYPECODE);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_configure(hand, XRIF_TESTLOOP_DIFFERENCE, XRIF_TESTLOOP_REORDER, XRIF_TESTLOOP_COMPRESS);
               ck_assert( rv == XRIF_NOERROR );
               
               hand->m_omp_parallel = 0;
               
               rv = xrif_allocate_raw(hand);
               ck_assert( rv == XRIF_NOERROR );
      
               rv = xrif_allocate_reordered(hand);
               ck_assert( rv == XRIF_NOERROR );
      
               XRIF_TESTLOOP_TYPE * buffer = (XRIF_TESTLOOP_TYPE *) hand->m_raw_buffer;
               
               rv = XRIF_TESTLOOP_FILL_FUNC(buffer, hand->m_width*hand->m_height*hand->m_frames,q);
               
               ck_assert( rv == 0 );
               
               XRIF_TESTLOOP_TYPE * compbuff = (XRIF_TESTLOOP_TYPE *) malloc( hand->m_width*hand->m_height*hand->m_frames*sizeof(XRIF_TESTLOOP_TYPE));
               memcpy(compbuff, buffer, hand->m_width*hand->m_height*hand->m_frames*sizeof(XRIF_TESTLOOP_TYPE));
      
               rv = XRIF_TESTLOOP_ENCODE(hand);
               ck_assert( rv == 0 );
               
               rv = xrif_set_compress_method_direction( hand, XRIF_TESTLOOP_COMPRESS, XRIF_DIRECTION_DECOMPRESS);
               ck_assert( rv == 0 );

               rv = XRIF_TESTLOOP_DECODE(hand);
               ck_assert( rv == 0 );
      
               int neq = 0;
               for( size_t i = 0 ; i < hand->m_width*hand->m_height*hand->m_frames ; ++i ) 
               {
                  if(buffer[i] != compbuff[i]) 
                  {
                     ++neq;
                  }
               }
               
               if(neq > 0)
               {
                  ++fail;
                  fprintf(stderr, "failure: " XRIF_TESTLOOP_DIFF_STR "/" XRIF_TESTLOOP_REORD_STR "/" XRIF_TESTLOOP_COMP_STR " %d %d %d\n", ws[w], hs[h], ps[p]);
               }
                              
               free(compbuff);
               
               dt_difference += xrif_difference_time(hand);
               dt_reorder += xrif_reorder_time(hand);
               dt_compress += xrif_compress_time(hand);
               dt_encode += xrif_encode_time(hand);
               dt_ratio += xrif_compression_ratio(hand);
               dt_decompress += xrif_decompress_time(hand);
               dt_unreorder += xrif_unreorder_time(hand);
               dt_undifference += xrif_undifference_time(hand);
               dt_decode += xrif_decode_time(hand);
               
             
               xrif_reset(hand);
            }//q
            
            #ifndef XRIF_TESTLOOP_NOPERF
            size_t bytes = ws[w]*hs[h]*ps[p]*sizeof(XRIF_TESTLOOP_TYPE);
            printf(XRIF_TESTLOOP_DIFF_STR "/" XRIF_TESTLOOP_REORD_STR "/" XRIF_TESTLOOP_COMP_STR " %d x %d x %d:\n", ws[w], hs[h], ps[p]);
            printf("   ratio:           %0.2f %%\n", dt_ratio/test_trials*100);
            printf("   encode:          %0.2f MB/s\n", bytes/(dt_encode/test_trials)/1048576);
            printf("      difference:   %0.2f MB/s\n", bytes/(dt_difference/test_trials)/1048576);
            printf("      reorder:      %0.2f MB/s\n", bytes/(dt_reorder/test_trials)/1048576);
            printf("      compress:     %0.2f MB/s\n", bytes/(dt_compress/test_trials)/1048576);
            printf("   decode:          %0.2f MB/s\n", bytes/(dt_decode/test_trials)/1048576);            
            printf("      decompress:   %0.2f MB/s\n", bytes/(dt_decompress/test_trials)/1048576);
            printf("      unreorder:    %0.2f MB/s\n", bytes/(dt_unreorder/test_trials)/1048576);
            printf("      undifference: %0.2f MB/s\n", bytes/(dt_undifference/test_trials)/1048576);
            #endif
         }//p
      }//h
   }//w
   
   
   ck_assert( fail == 0 );
   
   rv = xrif_delete(hand);
   ck_assert( rv == XRIF_NOERROR );
   
#undef XRIF_TESTLOOP_TYPECODE   
#undef XRIF_TESTLOOP_TYPE
#undef XRIF_TESTLOOP_DIFFERENCE 
#undef XRIF_TESTLOOP_DIFF_STR
#undef XRIF_TESTLOOP_REORDER 
#undef XRIF_TESTLOOP_REORD_STR   
#undef XRIF_TESTLOOP_COMPRESS 
#undef XRIF_TESTLOOP_COMP_STR
#undef XRIF_TESTLOOP_FILL_FUNC
#undef XRIF_TESTLOOP_ENCODE
#undef XRIF_TESTLOOP_DECODE
#undef XRIF_TESTLOOP_NOPERF
