# xrif

A fast high performance compression system.

# Building

## Dependencies

The only dependencies are the `check` unit testing framework and `subunit` streaming system.

On CentOS:
```
$ sudo yum install check-devel
$ sudo yum install subunit-devel
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

rv = xrif_set_size(xrif, rows, cols, 1, planes, XRIF_TYPECODE_INT16); // For a cube with `planes` images of size `rows X cols` `int int16_t`

rv = xrif_allocate(xrif);

memcpy( xrif->raw, source_data, rows*cols*planes*sizeof(int16_t));

rv = xrif_encode(xrif);

rv = xrif_decode(xrif);

memcpy(source_data, xrif->raw, rows*cols*planes*sizeof(int16_t));

```

# Header Format

| Field | Bytes |  value
|-------|-------|-----------------
| 0     | 0-3   | `'x' 'r' 'i' 'f'` [magic number]
| 1     | 4-7   | `uint32_t` version number of xrif protocol
| 2     | 8-11  | `uint32_t` total size of header [offset to beginning of data, minimum is 48]
| 3     | 12-15 | `uint32_t` width of a frame
| 4     | 16-19 | `uint32_t` height of a frame
| 5     | 20-23 | `uint32_t` depth of a frame [allows cubes, IFU spectra, etc]
| 6     | 24-27 | `uint32_t` number of frames
| 7     | 28-29 | `uint16_t` typecode [is uint8_t in struct, but alignment]
| 7     | 30-31 | `uint16_t` difference method
| 8     | 32-33 | `uint16_t` reorder method
| 9     | 34-35 | `uint16_t` compression method
| 10    | 36-47 | Reserved, used for method specific parameters. 

The current version is `0`

The size of the data is specified by `width X height X depth X xrif_typesize(typecode) X frames`

Difference method can be:

| Val. | Meaning
|------|---------
|  0   |none
|  100 | w.r.t. previous frame
|  200 | w.r.t. first frame

Reorder method can be:

| Val. | Meaning
|------|---------
| 0    | none
| 100  | bytepack
| 200  | bytepack-renibble
| 300  | bitpack

Compression method can be:

| Val. | Meaning
|------|---------
| 0    | none
| 100  | LZ4

If Compression method is LZ4 then bytes 36-37 are `uint16_t` containing the `lz4_acceleration` parameter.

