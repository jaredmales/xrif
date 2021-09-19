
/** \defgroup xrif_interface User Interface 
  * 
  * \brief The user interface to the XRIF library.
  */
      /** \defgroup xrif_overview Interface Overview 
        * 
        * \brief Overview of how to use the XRIF library.
        * 
        * \ingroup xrif_interface
        */
      /** \defgroup config_macros Configuration Values
        * \brief The typedefs and constants used to configure xrif functionality 
        * 
        * \ingroup xrif_interface
        */
            /** \defgroup xrif_types Typedefs 
              * \brief the types used by xrif
              * \ingroup config_macros
              */
            /** \defgroup diff_methods Difference Methods
              * \brief constants defining the difference methods
              * 
              * These are the values used to set the difference methods
              * \ingroup config_macros
              */
            /** \defgroup reorder_methods Reordering Methods
              * \brief constants defining the reordering methods
              * 
              * These are the values used to set the reordering methods
              ** \ingroup config_macros
              */
            /** \defgroup compress_methods Compression Methods
              * \brief constants defining the compression methods
              * 
              * These are the values used to set the compression methods
              ** \ingroup config_macros
              */
                  /** \defgroup lz4_config LZ4 Configuration
                    * \ingroup compress_methods
                    */
                  /** \defgroup lz4hc_config LZ4HC Configuration
                    * \ingroup compress_methods
                    */
                  /** \defgroup fastlz_config FastLZ Configuration
                    * \ingroup compress_methods
                    */
                  /** \defgroup zstd_config zstd Configuration
                    * \ingroup compress_methods
                    */
                  /** \defgroup zlib_config zlib configuration 
                    * \ingroup compress_methods
                    */
            /** \defgroup compress_directions Compression Directions
              * \brief constants defining the compression directions
              * 
              * These are the values used to set the compression direction
              * \ingroup config_macros
              */
            /** \defgroup error_codes Error Codes
              * \brief Error codes used by the xrif library
              * 
              * This library defines an error code type (merely an int) and a number of codes, all less than zero, to report errors.
              * In general we avoid in-band error reporting, with a few exceptions (e.g. xrif_typesize).
              * \ingroup config_macros
              */
            /** \defgroup typecodes Type Codes 
              * \brief Type codes for storing the type of the data.  
              * 
              * These are identical to the ImageStreamIO data types.
              * \ingroup config_macros
              */
      /** \defgroup xrif_init Initialization, Setup, and Allocation 
        * \ingroup xrif_interface
        */
             /** \defgroup xrif_init_fine Fine-grained Initialization, Setup, and Allocation 
               * \ingroup xrif_init
               */
      /** \defgroup access Current Configuration
        * \brief Access to current configuration values.
        * \ingroup xrif_interface
        */
      /** \defgroup header Header Processing 
        * \brief Functions to read and write the xrif header.
        * \ingroup xrif_interface
        */
      /** \defgroup xrif_encode_decode Encoding & Decoding
        * \brief These functions perform a complete cycle of encode and decode.
        * \ingroup xrif_interface
        */ 
            /** \defgroup xrif_diff Differencing
              * \ingroup xrif_encode_decode
              */
                  /** \defgroup xrif_diff_previous Previous Differencing
                    * 
                    * The previous differencing method uses the previous frame as the reference.
                    * \ingroup xrif_diff
                    */ 
                  /** \defgroup xrif_diff_first First Differencing
                    *
                    * The first differencing method uses the first frame as the reference for all subsequent frames.
                    * \ingroup xrif_diff
                    */
                  /** \defgroup xrif_diff_pixel Pixel Differencing
                    * 
                    * The pixel differencing method uses the previous pixel as the reference.
                    * \ingroup xrif_diff
                    */
                  /** \defgroup xrif_diff_bayer Bayer Differencing
                    * 
                    * Bayer filter differencing for RGGB type pixel formats.
                    * \ingroup xrif_diff
                    */
            /** \defgroup xrif_reorder Reordering
              * \ingroup xrif_encode_decode 
              */
                  /** \defgroup xrif_reorder_none No Reordering
                    *
                    * Functions to perform the no-reordering operations 
                    * \ingroup xrif_reorder
                    */
                  /** \defgroup xrif_reorder_bytepack Bytepack Reordering
                    * 
                    * Functions to perform the "bytepack" reordering method
                    * \ingroup xrif_reorder 
                    */
                  /** \defgroup xrif_reorder_bytepack_renibble Bytepack-Renibble Reordering
                    * 
                    * Functions to perform the "bytepack-renibble" reordering method
                    * \ingroup xrif_reorder 
                    */
                  /** \defgroup xrif_reorder_bitpack Bitpack Reordering
                    * 
                    * Functions to perform the "bitpack" reordering method
                    * \ingroup xrif_reorder 
                    */
            /** \defgroup xrif_compress Compression
              * \ingroup xrif_encode_decode
              */
                 /** \defgroup compress_none No Compression
                   * \ingroup xrif_compress
                   */
                  /** \defgroup compress_lz4 LZ4 Compression
                   * \ingroup xrif_compress
                   */
                  /** \defgroup compress_lz4hc LZ4HC Compression
                   * \ingroup xrif_compress
                   */
                  /** \defgroup compress_fastlz FastLZ Compression
                   * \ingroup xrif_compress
                   */
                  /** \defgroup compress_zstd zstd Compression
                   * \ingroup xrif_compress
                   */
                  /** \defgroup compress_zlib zlib Compression
                   * \ingroup xrif_compress
                   */
      /** \defgroup xrif_performance Performance Measurements
        * \ingroup xrif_interface
        */ 
      /** \defgroup xrif_utils Utility Functions
        * \ingroup xrif_interface
        */ 

/** \defgroup xrif_hacking Development  
  * 
  * \brief Developing the XRIF library.
  */