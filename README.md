# xrif

A fast, high performance, lossless compression system.

Code documentation is here: https://jaredmales.github.io/xrif-docs/modules.html

# Building

## Dependencies

The only dependencies are the `check` unit testing framework and `subunit` streaming system.

On CentOS:
```
$ sudo yum install check-devel
$ sudo yum install subunit-devel
```

On Ubuntu:
```
$ sudo apt-get install check
```

## Configure, Make, and Install

For a standard posix system, you should only need to do these steps:
```
$ mkdir build 
$ cd build 
$ cmake ..
$ make
$ make test
$ sudo make install
$ sudo ldconfig
```
# Basic Usage
The below code shows the steps needed to initialize an xrif handle and use it to compress a cube of images.

```
int16_t * source_data;
// ... allocate and populate source_data with `planes` images of size `rows X cols`

xrif_t xrif;
xrif_error_t rv;
rv = xrif_new(&xrif);

rv = xrif_set_size(xrif, rows, cols, 1, planes, XRIF_TYPECODE_INT16); // For a cube with `planes` images of size `rows X cols` of type `int16_t`

rv = xrif_allocate(xrif);

memcpy( xrif->raw, source_data, rows*cols*planes*sizeof(int16_t));

rv = xrif_encode(xrif);

rv = xrif_decode(xrif);

memcpy(source_data, xrif->raw, rows*cols*planes*sizeof(int16_t));

```

The library provides ways to use `source_data` directly (avoiding the memcpy), and control over which algorithms are used at each step of the compression.

Note that only `int16` monochrome images are fully supported at this time.

# Header Formats

The current writer emits **version 1**, with a **56-byte header** and an unsigned **64-bit compressed payload length** (up to `UINT64_MAX`). Width, height, depth, and frame count each remain unsigned 32-bit values; their products use checked arithmetic. The reader also accepts the legacy **version-0, 48-byte header** with its 32-bit payload length.

| Format | Header byte order | Payload-length field | Codec-options field |
| --- | --- | --- | --- |
| Version 0 | Writer's native byte order | Bytes 36–39, `uint32_t` | Bytes 40–47 |
| Version 1 | Little-endian | Bytes 40–47, `uint64_t` | Bytes 48–55 |

See [the complete header specification](doc/header-format.md) for every field's byte offsets, signed method identifiers, codec options, size limits, and migration guidance. Payload lengths exclude the header. Host address-space and codec limits still apply.

Allocate new headers using `XRIF_HEADER_SIZE`, and prefer the capacity-checked `xrif_write_header_sized` / `xrif_read_header_sized` APIs. Recompile callers that used a hard-coded 48-byte writer buffer. Older libraries reject version 1; the new reader retains version-0 support.

# Code Documentation

The code documentation is here: [https://jaredmales.github.io/xrif/](https://jaredmales.github.io/xrif/) 
