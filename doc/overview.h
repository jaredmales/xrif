/** \addtogroup xrif_overview
  * 
  * The following code illustrates how the XRIF library should used to compress data under most circumstances.  
  * Lower level access is provided if fine-grain control is needed.
  *
  * First, create the xrif library handle:
  * \code
  * xrif_error_t rv;
  * xrif_t handle; //note: this is a pointer type
  * rv = xrif_new(&handle); //note: the argument is a pointer to pointer
  * //check rv for errors here . . .
  * \endcode
  * You now have an initialized xrif handle.  The `xrif_t` type can not be used until `xrif_new` has been called on it.
  * Never call `xrif_new` twice on the same handle, unless `xrif_delete` (see below) has been called.
  * 
  * Next you need to configure and allocate the handle:
  * \code
  * rv = xrif_set_size(handle, 64,64, 1,1024, XRIF_TYPECODE_INT16); 
  * rv = xrif_configure(handle, XRIF_DIFFERENCE_PREVIOUS, XRIF_REORDER_BYTEPACK, XRIF_COMPRESS_LZ4); //Note this is optional if defaults are ok.
  * rv = xrif_allocate(handle);
  * \endcode
  * The above results in an xrif handle prepared to operate on 1024 signed 16-bit images of size 64x64.  (Note that you should check the value of rv for errors!)
  * 
  * Now to compress data:
  * \code
  * memcpy(handle->raw_buffer, my_source, xrif_raw_size(handle) ); //You are responsible for `my_source`.
  * rv = xrif_encode(handle);
  * printf("%f\%\n", handle->compression_ratio*100.0);
  * \endcode
  * After this `handle->raw_buffer` contains the compressed data.  This is because `handle->compress_on_raw == 1` by default, causing the `raw_buffer` to be re-used.
  * This also illustrates the built in performance monitoring, which is on by default but can be turned off.
  * 
  * To decompress:
  * \code
  * rv = xrif_decode(handle);
  * memcpy(my_dest, xrif->raw_buffer, xrif_raw_size(xrif)); //You are responsible for `my_dest`.
  * \endcode
  * after which `my_dest` will contain the original data.
  * 
  * To start over, use:
  * \code
  * rv = xrif_reset(handle);
  * \endcode
  * 
  * And when you are done with the handle, it can be fully de-allocated with
  * \code
  * rv = xrif_delete(handle);
  * \endcode
  * after which the handle should not be used again, unless `xrif_new` is called on it.
  * 
  */