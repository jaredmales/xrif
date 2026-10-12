# XRIF header formats {#xrif_header_formats}

The current writer emits **version 1**. The reader accepts version 0 and version 1. Both retain four unsigned 32-bit axes: width, height, depth, and frames. Their products and byte sizes are checked against `UINT64_MAX` and the host's `SIZE_MAX` before use.

The compressed-length field describes the **entire payload across all frames**, in bytes, excluding the header. Its maximum representable value is `UINT32_MAX` (4,294,967,295) in version 0 and `UINT64_MAX` (18,446,744,073,709,551,615) in version 1. This is a format limit, not a promise that a host can allocate that amount of memory or a particular codec can process it.

## Byte layouts

Offsets are zero-based and inclusive. These are byte layouts, not C structs; readers and writers must not rely on struct padding or buffer alignment.

| Field | Type | Version 0 bytes | Version 1 bytes |
| --- | --- | --- | --- |
| Magic | Four ASCII bytes `xrif` | 0–3 | 0–3 |
| Protocol version | `uint32_t`, 0 or 1 | 4–7 | 4–7 |
| Total header length / payload offset | `uint32_t` | 8–11, at least 48 | 8–11, at least 56 |
| Width, pixels per row | `uint32_t` | 12–15 | 12–15 |
| Height, rows per plane | `uint32_t` | 16–19 | 16–19 |
| Depth, planes per frame | `uint32_t` | 20–23 | 20–23 |
| Frames | `uint32_t` | 24–27 | 24–27 |
| Element type code | `uint16_t` | 28–29 | 28–29 |
| Difference method | `int16_t` | 30–31 | 30–31 |
| Reorder method | `int16_t` | 32–33 | 32–33 |
| Compression method | `int16_t` | 34–35 | 34–35 |
| Compressed payload length | `uint32_t` in v0; `uint64_t` in v1 | 36–39 | 40–47 |
| Reserved, written as zero | Four bytes | — | 36–39 |
| Codec options | Eight bytes, described below | 40–47 | 48–55 |

Version 0 used the writer's **native byte order** for multi-byte fields and had no endianness marker. Legacy files must therefore be read with the same byte order as their writer. Version 1 defines **little-endian** header integers, with signed fields encoded in two's complement. Its encoder writes reserved and unused option bytes as zero; readers ignore them. This header convention does not convert the image payload's element byte order or make existing reorder algorithms endian-independent.

The version-1 writer emits exactly 56 bytes (`XRIF_HEADER_SIZE`); `XRIF_HEADER_SIZE_V0` is 48. If a header declares an extension, its total length includes that extension, and payload data begins at the declared offset. Readers skip unknown extension bytes after the fixed fields. Header length remains a 32-bit value and is separate from the 64-bit payload length. A caller computing a combined allocation or file offset must also check **header length + payload length** for overflow.

## Codec options

Offsets here are relative to the beginning of the eight-byte options area: byte 40 in version 0 or byte 48 in version 1. Multi-byte options follow the corresponding version's byte order.

| Compression method | Option offset | Type | Meaning |
| --- | --- | --- | --- |
| NONE (-1) | — | — | All option bytes unused |
| LZ4 (100) | 0–1 | `uint16_t` | Acceleration |
| LZ4HC (102) | 0–1 | `uint16_t` | Compression level |
| FastLZ (110) | 0–1 | `uint16_t` | Compression level, 1 or 2 |
| Zstandard (120) | 0–3 | `int32_t` | Compression level, which may be negative |
| zlib (130) | 0 | `uint8_t` | Compression level |
| zlib (130) | 1 | `uint8_t` | Compression strategy |

Remaining option bytes are unused. There is no additional method metadata in the header.

## Type and method identifiers

| Type code | Element type | Bytes per element |
| --- | --- | --- |
| 1 / 2 | Unsigned / signed 8-bit integer | 1 |
| 3 / 4 | Unsigned / signed 16-bit integer | 2 |
| 5 / 6 | Unsigned / signed 32-bit integer | 4 |
| 7 / 8 | Unsigned / signed 64-bit integer | 8 |
| 9 | Float | 4 |
| 10 | Double | 8 |
| 11 | Complex float | 8 |
| 12 | Complex double | 16 |
| 13 | Half | 2 |

These are size/type identifiers; not every transform supports every element type. Signed 16-bit monochrome data remains the fully supported transform path.

| Difference value | Meaning |
| --- | --- |
| -1 | NONE |
| 100 | PREVIOUS0: previous-frame reference, first frame left unchanged (deprecated) |
| 101 | PREVIOUS: previous-frame reference with first-frame pixelL differencing |
| 200 | FIRST: first-frame reference |
| 300 | PIXEL0: preceding-pixel differencing |
| 310 | PIXELL: row/column differencing |

| Reorder value | Meaning |
| --- | --- |
| -1 | NONE |
| 100 | BYTEPACK |
| 200 | BYTEPACK_RENIBBLE |
| 300 | BITPACK (known broken; not supported for reliable use) |

The library's configuration shorthand `0` selects a default method; **it does not mean NONE**. Writers should serialize the configured method identifiers shown above.

## Sizes, codec limits, and failure behavior

Uncompressed bytes are `width × height × depth × frames × xrif_typesize(typecode)`. Each axis must be nonzero. Products and padding calculations use checked arithmetic; `xrif_set_size` returns `XRIF_ERROR_INVALID_SIZE` instead of wrapping if a result exceeds `min(UINT64_MAX, SIZE_MAX)`. It preserves the handle on failure and does not allocate or resize buffers on success. A 32-bit host rejects sizes that need a wider address space.

`xrif_set_size` is codec-independent. Subsequent method-bound queries return zero for invalid or unsupported sizes; allocation/processing rejects them before a codec receives narrowed arguments. NONE/BYTEPACK reorder capacity is the raw size. RENIBBLE uses `frame_bytes × (frames + 1)`; BITPACK's existing capacity formula is `max(frame_bytes, 16) × (frames + 1)`. These padded lengths, not just the raw length, feed the codec bound.

- **LZ4/LZ4HC:** one block is limited to bundled `LZ4_MAX_INPUT_SIZE` (2,113,929,216 input bytes). The API uses signed `int` lengths. XRIF does not add a chunked LZ4 container.
- **FastLZ:** signed `int` input/output limits apply, including the worst-case output bound. For inputs below the bundled API's documented 16-byte minimum, XRIF writes the same literal-run encoding directly, preserving tiny-image support without calling the backend on an unsupported length. Output capacity is at least 66 bytes and at least input plus 5% rounded upward.
- **zlib:** XRIF feeds a single zlib stream through input/output windows of at most `UINT_MAX` bytes, maintaining wide totals between calls. The bound query must fit `uLong`, `size_t`, and the checked 64-bit bound arithmetic. On ordinary 64-bit Linux this permits streams larger than 4 GiB.
- **Zstandard:** uses its native `size_t` interface and checked `ZSTD_compressBound` result. Backend size/object limits still apply; data calls reject extents exceeding `PTRDIFF_MAX`.
- **NONE:** uses the checked representation size without a codec-imposed 32-bit limit.

Buffer capacities describe actual caller-provided allocations. They must be truthful even for metadata that is numerically representable. Insufficient buffer capacity returns `XRIF_ERROR_INSUFFICIENT_SIZE` before traversal/copying; allocation failure returns `XRIF_ERROR_MALLOC`. Individual failed buffer replacements retain the previous storage and ownership. A multi-buffer allocation that runs out of memory after making progress remains safely destructible but is not transactionally rolled back.

## API migration and compatibility

Recompile callers and allocate header storage with `XRIF_HEADER_SIZE`, replacing any hard-coded 48-byte buffers. The existing C struct layout and function signatures remain intact, but the default emitted wire format changes. Old libraries reject version 1 with `XRIF_ERROR_WRONGVERSION`. The current writer does not offer version-0 output.

Prefer `xrif_write_header_sized(header, capacity, handle)` and `xrif_read_header_sized(handle, &header_size, header, available)` for new code. They accept unaligned buffers and check capacity before reading/writing fields. The reader validates magic, version, declared length, type, geometry, and compressed-length host representability before updating state. It leaves both the handle and `header_size` unchanged on failure. The writer leaves its destination unchanged on validation failure. Neither function accesses the payload.

The original `xrif_write_header` requires `XRIF_HEADER_SIZE` writable bytes. The original `xrif_read_header` requires the entire declared header to be readable; its signature cannot check the allocation length. Code reading from a file can first read the common 12-byte prefix, validate version and declared header length, then read the rest of that header before calling the sized reader. Do not assume every header is 56 bytes when reading old files.

Reading metadata updates dimensions, type, derived raw size, methods, and codec options. It preserves existing buffers, ownership, codec contexts, and direction. Configure decompression with `xrif_set_compress_direction(handle, XRIF_DIRECTION_DECOMPRESS)` before decoding with zlib or Zstandard, and allocate/check buffers for the newly loaded sizes.
