# XRIF bug audit — 2026-10-11

Audited commit: `bddd4beb8a798ec306c470584106a0937dbba1a0`.

This audit found **17 reproducible findings: five P1, eleven P2, and one P3**. The most urgent issues are undersized allocations caused by integer overflow, a double-free on an allocation error path, missing buffer bounds checks, and data corruption with parallel pixel differencing. No library, build, or test sources were changed.

## Follow-up — 2026-10-12

The original findings and reproductions below describe the audited commit and remain as historical evidence. The [checked-size/version-1 implementation record](2026-10-12-f01-checked-size-arithmetic.md) documents the subsequent repair, including the user-authorized move to a uint64_t payload length and the [two header layouts](../../../doc/header-format.md).

| Findings | Follow-up result |
| --- | --- |
| F01 | Checked geometry, padding, traversal extents/counters, and codec conversions; exact 64-bit metadata and overflow regression coverage |
| F02 | Rejected allocations retain prior ownership/storage; failed-reallocation/delete regression passes under ASan |
| F03 | Raw and reordered byte-capacity validation at active transform entry points; undersized-buffer regressions pass |
| F04 | All codec adapters validate advertised source length against actual capacity before decoding |
| F06 | Separate-buffer all-NONE encode/decode now copies the checked payload; both buffer modes tested |
| F07 | Both header versions reconstruct validated raw size; fresh-handle header/payload round trips pass |
| F14 | Byte operations/memcpy replace typed header accesses; unaligned header regressions pass |
| F16 | NONE decompression and the all-NONE shortcut reject incomplete payloads |
| F13 | Touched sizing/reorder paths now check null handles first; other configuration APIs remain open |
| F05, F08–F12, F15, F17 | Remain open; no claim that their independent behavior is repaired |

Verification includes 9,720 serial INT16 round trips, two boundary suites (11 checks each), selected four-worker OpenMP comparisons, OpenMP-disabled builds, and C11/C++17 static/shared consumers. Full CTest has seven passing targets and the existing stale target not run. ASan reports no new invalid access; UBSan still reports bundled FastLZ alignment accesses. LeakSanitizer and real 32-bit execution were unavailable. See the implementation record for exact limits and build workarounds.

## Scope and validation

Reviewed the tracked C library, compression adapters, header serialization, buffer ownership, CMake configuration, and tests. The pre-existing untracked `benchmarks/` directory and old `_build/` binaries were excluded. This is a targeted correctness audit, not an exhaustive security review of the bundled compression libraries.

Validation used Linux x86-64, GCC 15.2, OpenMP, and fresh builds under `/tmp`. The host lacked `zstd.h`; `libzstd-dev` 1.5.7 was downloaded and extracted under `/tmp/xrif-audit-deps` without installing system packages. The normal CMake build succeeded after supplying that include path and the existing Zstandard shared library. CMake appends `-O3` even for these Debug builds.

- CTest: **five passed, one not run** because its executable name is stale (F11). The registered increment and white-noise suites passed.
- Manually ran the three omitted `previous0`, `pixel0`, and `pixelL` difference test executables: each passed all six checks.
- A separate serial `INT16` matrix passed **9,720 encode/decode comparisons**: widths 1–5, heights 1–3, depths 1–2, frames 1–3, all six difference settings, NONE/BYTEPACK/BYTEPACK_RENIBBLE reordering, and all six compression settings. It used compression on the raw buffer and explicitly selected decompression direction before decoding.
- The same matrix completed under AddressSanitizer/UndefinedBehaviorSanitizer without an ASan failure. UBSan emitted alignment diagnostics in bundled FastLZ; this was **not** a clean UBSan run.
- Separate small probes reproduced the findings below, including ASan buffer violations and a double-free, and UBSan signed-overflow and header-alignment violations. The probe source is embedded in the appendix so the evidence does not depend on temporary files surviving.

P1 means urgent memory safety or silent corruption; P2 means a functional, integration, or portability defect; P3 means an incorrect diagnostic metric. Priority is conditional on the trigger described for each finding.

## Findings

| ID | Priority | Finding | Evidence |
| --- | --- | --- | --- |
| F01 | P1 | Dimension products overflow before conversion to `size_t` | Incorrect size and sanitizer-detected invalid access |
| F02 | P1 | Failed compressed-buffer reallocation leaves a dangling owned pointer | ASan double-free |
| F03 | P1 | Differencing and inverse reordering fail to enforce byte capacities | ASan heap read/write violations |
| F04 | P1 | Decompression trusts a compressed length larger than its source allocation | ASan heap read violation |
| F05 | P1 | OpenMP pixel differencing violates iteration dependencies | 100/100 corrupt parallel round trips |
| F06 | P2 | All-NONE shortcut ignores a separate compressed buffer | Successful calls leave output unchanged |
| F07 | P2 | Header loading never reconstructs `m_raw_size` | Reports zero for a 48-byte image |
| F08 | P2 | 32-bit bytepack accepts input but loses bytes and cannot decode | Wrong byte stream; inverse returns NOTIMPL |
| F09 | P2 | Zlib setters do not update an existing compression stream | Requested level 0 still compresses |
| F10 | P2 | Library targets omit zlib and Zstandard link dependencies | Minimal shared-library client fails to link |
| F11 | P2 | CTest registers a nonexistent test and omits replacement tests | Fresh CTest run fails with Not Run |
| F12 | P2 | 32-/64-bit difference arithmetic uses overflowing signed operations | UBSan signed-overflow diagnostics |
| F13 | P2 | Some public null-handle checks occur after dereferencing the handle | Sanitizer-confirmed null dereference |
| F14 | P2 | Header access requires undocumented integer alignment | UBSan misaligned loads/stores |
| F15 | P2 | Invalid compression configuration reports success | Incorrect return values and retained invalid direction |
| F16 | P2 | Uncompressed decoder accepts truncated payloads as complete images | Successful decode of 1 byte instead of 8 |
| F17 | P3 | Difference and reorder performance timings are swapped | Deterministic timestamp probe |

### F01 — Dimension multiplication wraps in 32 bits

**Location:** [src/xrif.c](../../../src/xrif.c), lines 145–166 and 551–599; related products occur throughout the transforms. `xrif_dimension_t` is `uint32_t` ([src/xrif.h](../../../src/xrif.h), line 138).

`w*h*d*f*sz` evaluates its dimension-only prefix as 32-bit unsigned arithmetic. Assigning the result to `size_t`, or multiplying by `sz` last, does not recover lost bits. Allocation helpers repeat the same pattern, while some subsequent traversal expressions operate in `size_t` and visit the full dimensions.

**Reproduce:** Probe `overflow` calls `xrif_set_size(x, 65536, 65537, 1, 1, XRIF_TYPECODE_INT16)`. It succeeds and reports `131072` raw bytes instead of `8590065664`; minimum raw allocation is only `131602` bytes. Probe `overflow crash` then successfully allocates that small buffer and calls default differencing. ASan detects an invalid read at `xrif_difference_previous.c:150`.

**Fix:** Use one checked multiplication routine, widen before the first multiplication, and reject overflow before changing the handle. Apply it to header-derived sizes, intermediate strides, extra-frame calculations, and codec limits; casting only the final product is insufficient.

### F02 — Allocation failure makes normal cleanup double-free

**Location:** [src/xrif.c](../../../src/xrif.c), lines 878–893; cleanup at lines 263–265. The raw/reordered allocation functions have analogous free-before-validation patterns at lines 708–718 and 793–803.

`xrif_allocate_compressed` frees the existing owned buffer before checking whether the new configuration has a valid allocation size. Its early returns preserve the freed pointer and ownership flag. A subsequent reset, delete, or retry can free it again.

**Reproduce:** Probe `double_free` allocates a small handle with `m_compress_on_raw=0` and LZ4, changes the size to `1073741824 × 1 × 1 × 1` `INT16`, and calls `xrif_allocate_compressed`. LZ4 rejects the 2 GiB input bound, so the allocator returns `XRIF_ERROR_INVALID_SIZE` (`-20`). Calling `xrif_delete` to clean up then triggers ASan's `attempting double-free`. The test never allocates 2 GiB.

**Fix:** Validate and allocate a replacement before releasing the old buffer, or clear the pointer, size, and ownership immediately after freeing. Ensure every failure leaves a safely destructible handle.

### F03 — Runtime capacity checks compare pixels to bytes or are absent

**Location:** [src/xrif_difference_previous.c](../../../src/xrif_difference_previous.c), lines 341–350 and 630–639; the same missing element-size factor appears in `first`, `previous0`, `pixel0`, and `pixelL`. [src/xrif_reorder_bytepack.c](../../../src/xrif_reorder_bytepack.c), lines 300–346, and the renibble inverse lack buffer-size checks altogether.

The difference dispatchers compare the byte capacity against the pixel count instead of `pixel_count * element_size`. Buffers between those two sizes pass validation and are accessed out of bounds. Inverse bytepack accesses raw/reordered buffers without checking either capacity. These paths contradict the advertised insufficient-size error behavior of encode/decode.

**Reproduce:** Probe `short_raw` allocates a two-pixel `INT16` raw buffer with compression stored separately, changes the configured width to four without reallocating, and calls differencing. Capacity is four bytes and the required size is eight; ASan catches a read beyond the allocation instead of an insufficient-size return. Probe `short_unreorder` additionally allocates the correctly sized reordered buffer and calls inverse bytepack; ASan catches a write at the end of the four-byte raw allocation.

The trigger is a stale or undersized buffer after configuration changes, not an ordinary correctly allocated round trip.

**Fix:** Before any transform, validate both pointers and checked byte counts against actual capacities. Apply equivalent validation to public direct transform functions and to decoding.

### F04 — Decoder passes an unchecked source length to compression libraries

**Location:** [src/xrif_compress_lz4.c](../../../src/xrif_compress_lz4.c), lines 206–217; analogous calls in the LZ4HC, FastLZ, zlib, and Zstandard adapters. Header metadata is assigned directly to `m_compressed_size` in [src/xrif.c](../../../src/xrif.c), line 1182.

The decoder selects a source pointer but never compares `m_compressed_size` with the corresponding `m_raw_buffer_size` or `m_compressed_buffer_size`. A bounded third-party decoder can only enforce the length XRIF supplies; it cannot discover that the actual allocation is smaller. Corrupt header metadata or an inconsistent caller-supplied length can therefore cause an out-of-bounds read.

**Reproduce:** Probe `oversized_input` allocates four `INT16` pixels using BYTEPACK/LZ4, giving a 24-byte raw/compressed allocation. Fill it with `0xff`, set `m_compressed_size=1000`, and call `xrif_decode`. ASan reports a heap-buffer-overflow in LZ4's `read_variable_length`, reached from `xrif_decompress_lz4:217`.

**Fix:** Reject source lengths exceeding the selected buffer's actual capacity before calling any backend, and range-check conversions to codec integer sizes. This complements, rather than replaces, the caller's responsibility to verify the number of bytes read from a file or stream.

### F05 — Parallel PIXEL0 is not lossless

**Location:** [src/xrif_difference_pixel0.c](../../../src/xrif_difference_pixel0.c), lines 133–144 and 319–330, with the same pattern for other integer widths.

Forward differencing reads an adjacent element that another thread may already have modified. Inverse differencing is a prefix sum: each iteration needs the completed result from the preceding iteration. `omp for` does not preserve either dependency across thread partitions.

**Reproduce:** Probe `omp_pixel` creates a `4096 × 1` `INT16` frame with pixels `0..4095`, uses `omp_set_num_threads(4)`, and performs difference/undifference 100 times. With `m_omp_parallel=0`, all round trips match. With it set to 1, **100/100 differ** although both API calls return success.

**Fix:** Keep each dependent pixel chain serial, parallelize independent frames/planes, or implement a correct parallel difference/scan algorithm with preserved boundary values. Add a round-trip test that actually enables multiple OpenMP workers.

### F06 — All-NONE shortcut drops separate-buffer input/output

**Location:** [src/xrif.c](../../../src/xrif.c), lines 1218–1234 and 1297–1309.

When all three methods are NONE, encode/decode bypass the copy stages regardless of `m_compress_on_raw`. This is only appropriate when the raw buffer is also the compressed buffer. Separate-buffer callers receive success even though encoding never fills their output and decoding never restores the raw data.

**Reproduce:** Probe `none` configures four `INT16` pixels, all methods NONE, and `m_compress_on_raw=0`. With raw bytes `0x42` and compressed bytes `0xcc`, encode reports eight bytes but compressed byte zero stays `cc`. With compressed bytes `0x42` and raw bytes zero, decode succeeds and raw byte zero stays `00`.

**Fix:** Copy exactly the checked payload length between the selected buffers in this shortcut, or use the ordinary NONE stages when compression is stored separately.

### F07 — Header loading leaves the raw-size accessor zero or stale

**Location:** [src/xrif.c](../../../src/xrif.c), lines 1164–1182; [src/xrif.h](../../../src/xrif.h), line 474, says `m_raw_size` is set from the header.

`xrif_read_header` assigns dimensions, type, and element size but never updates `m_raw_size`. A fresh handle reports zero; a reused handle retains the size of its previous image. Consumers using `xrif_raw_size` for allocation/copying and performance metrics get incorrect results even when decoding itself succeeds.

**Reproduce:** Probe `header` writes and reads a valid header for `4 × 3 × 1 × 2` `INT16`. The new handle reports `raw_size=0`, expected 48, for LZ4, zlib, and Zstandard. Selecting decompression direction explicitly allows each payload to decode but does not repair the missing raw size.

**Fix:** Reconstruct and validate all size-derived state through the same checked sizing routine used by `xrif_set_size`.

### F08 — 32-bit bytepack succeeds with an irreversible representation

**Location:** [src/xrif_reorder_bytepack.c](../../../src/xrif_reorder_bytepack.c), lines 229–232, 267–270, and 370–373.

The 32-bit encoder indexes input with `2*pix` instead of `4*pix`. It overlaps words and omits later input bytes. The corresponding inverse dispatcher also unconditionally returns `XRIF_ERROR_NOTIMPL`, so a successful encode cannot be decoded by this implementation.

**Reproduce:** Probe `bytepack32` supplies four `INT32` pixels with raw bytes `00..0f`. Reordering succeeds but produces `00 02 04 06 / 01 03 05 07 / 02 04 06 08 / 03 05 07 09`, losing bytes `0a..0f`. Inverse reordering returns `-100`.

The README limits full support to 16-bit images; this finding concerns the public 32-bit path that nevertheless accepts and successfully encodes data.

**Fix:** Implement matching four-byte encode/decode paths with full-width round-trip coverage, or reject 32-bit bytepack before modifying the input.

### F09 — Zlib parameter setters update metadata but not the active stream

**Location:** [src/xrif_compress_zlib.c](../../../src/xrif_compress_zlib.c), lines 198–254; stream initialization at line 123 and reuse at lines 363–380.

Selecting zlib initializes its stream immediately. Later level/strategy setters only update handle fields, while `deflateReset` preserves the stream's previous compression parameters. Getters and serialized metadata then claim settings the encoder is not using.

**Reproduce:** Probe `zlib_level` initializes zlib for 65,536 zero bytes, then calls `xrif_set_zlib_level(x,0)` and `xrif_set_zlib_strategy(x,Z_DEFAULT_STRATEGY)`. Both succeed, but encoding produces **85 bytes**, still using the old compressed settings. Re-selecting zlib to reinitialize the stream produces **65,552 bytes** for the same input with the requested level 0.

**Fix:** Reconfigure/reinitialize an existing compression stream when either setting changes, with a defined policy for pending stream data and errors.

### F10 — Built shared library omits its codec dependencies

**Location:** [src/CMakeLists.txt](../../../src/CMakeLists.txt), lines 31–37; dependency discovery/linking exists only in [tests/CMakeLists.txt](../../../tests/CMakeLists.txt).

Neither library target links zlib or Zstandard. Linux permits building the shared object with unresolved symbols, so the successful library build and explicitly linked tests hide the problem. A normal consumer linking `-lxrif` fails with undefined references to `inflate`, `deflate`, `ZSTD_decompressDCtx`, and other codec functions. Dynamic loading also depends on the caller having independently loaded these libraries.

**Reproduce:** Compile the minimal `xrif_new`/`xrif_delete` client in the appendix against the fresh shared library using `-lxrif -fopenmp`. Linking fails. `readelf -d libxrif.so` lists only `libgomp.so.1` and `libc.so.6` as needed libraries, with no zlib or Zstandard.

**Fix:** Discover required codec dependencies at library configuration time and attach them to the library targets, including transitive requirements for static consumers. Supply required include paths there and update the README's dependency list, which currently mentions only Check/Subunit.

### F11 — Fresh CTest run cannot execute the registered pixel test

**Location:** [CMakeLists.txt](../../../CMakeLists.txt), lines 35–40, versus [tests/CMakeLists.txt](../../../tests/CMakeLists.txt), lines 10–26.

CTest still registers `xrif_test_difference_pixel_whitenoise`, but the built targets are `xrif_test_difference_pixel0_whitenoise` and `xrif_test_difference_pixelL_whitenoise`. It also omits the existing `previous0` and ASCII executables. Stale binaries in an old build tree can conceal the missing executable.

**Reproduce:** `ctest --test-dir /tmp/xrif-audit-build --output-on-failure` exits unsuccessfully: five pass, `xrif_test_difference_pixel_whitenoise` is **Not Run**. Running the actual `previous0`, `pixel0`, and `pixelL` binaries directly passes all six checks in each.

**Fix:** Register existing targets using target names, remove the obsolete registration, and register the intended omitted suites with appropriate working directories/fixtures.

### F12 — Full-range 32-/64-bit differences have signed-overflow undefined behavior

**Location:** [src/xrif_difference_pixelL.c](../../../src/xrif_difference_pixelL.c), lines 210 and 463; equivalent signed additions/subtractions occur in the other 32-/64-bit difference implementations.

Lossless modular differencing needs defined wraparound, but these paths operate on `int32_t`/`int64_t`, including when the declared image type is unsigned. Differences and reconstruction sums can exceed the signed range. C does not guarantee wraparound for those operations, and the default optimized build does not request such semantics.

**Reproduce:** Probe `signed` uses PIXELL on two `INT32` values `{INT32_MIN, INT32_MAX}`, with reordering/compression disabled. UBSan reports both `2147483647 - -2147483648` and the inverse `-1 + -2147483648` as signed integer overflow.

As with F08, this affects implemented wider-integer APIs beyond the README's fully supported 16-bit case. The test proves undefined arithmetic, not a miscompilation on every compiler.

**Fix:** Perform reversible arithmetic in matching unsigned types and preserve bit patterns when storing results. Test minimum/maximum transitions for both signed and unsigned 32-/64-bit images.

### F13 — Public null-handle error paths can crash before checking

**Location:** [src/xrif.c](../../../src/xrif.c), line 540 (`xrif_set_compress_direction`), lines 1409/1430 (`xrif_reorder`/`xrif_unreorder`), and lines 1452/1487 (NONE transforms).

`xrif_set_compress_direction` reads `handle->m_compress_method` before delegating to the function that checks NULL. Reorder dispatchers dereference immediately; NONE transforms calculate `npix` before their explicit NULL guard. This prevents the expected `XRIF_ERROR_NULLPTR` return.

**Reproduce:** Probe `null` calls `xrif_set_compress_direction(NULL, XRIF_DIRECTION_DECOMPRESS)`. UBSan reports member access through NULL, followed by ASan's segmentation-fault report at `xrif.c:540`.

**Fix:** Check the handle before every dereference in public functions that promise null handling. Apply this consistently to the analogous dispatch and NONE paths.

### F14 — Header serialization dereferences potentially unaligned integers

**Location:** [src/xrif.c](../../../src/xrif.c), lines 1076–1119 and 1156–1203.

Header APIs accept `char *` buffers but cast offsets directly to `uint32_t *`, `uint16_t *`, and signed variants. No alignment requirement is documented. A header at a byte offset inside a packed file/network buffer can violate those pointer alignments, giving undefined behavior and possible faults on architectures requiring alignment.

**Reproduce:** Probe `align` allocates 49 bytes and passes `buffer+1` as the 48-byte header to both writer and reader. UBSan reports misaligned stores beginning at `xrif.c:1076` and corresponding loads in the reader. This does not require a short buffer.

**Fix:** Use `memcpy` to/from properly aligned scalar locals or explicit byte serialization. Define byte order separately; alignment-safe copying alone does not make the format endian-independent.

### F15 — Invalid compression configuration is accepted as successful

**Location:** [src/xrif.c](../../../src/xrif.c), lines 462–480; documented error contract in [src/xrif.h](../../../src/xrif.h), lines 793–794.

Unknown methods fall back to LZ4 but return success. For an invalid direction, the code mistakenly assigns the direction default to `m_compress_method`, then stores the invalid direction unchanged and can again return success. Callers cannot rely on the documented `XRIF_ERROR_BADARG`, and invalid state survives configuration.

**Reproduce:** Probe `badconfig`: `xrif_set_compress_method(x,123456)` returns 0 and selects method 100. `xrif_set_compress_method_direction(x,XRIF_COMPRESS_LZ4,123456)` returns 0 and leaves direction 123456.

**Fix:** Validate method/direction before shutting down existing contexts, set the documented fallback fields if that policy is retained, and return `XRIF_ERROR_BADARG` for rejected arguments.

### F16 — Truncated uncompressed payloads silently become zero-padded images

**Location:** [src/xrif.c](../../../src/xrif.c), lines 1659–1676.

`xrif_decompress_none` only checks whether the destination can hold `m_compressed_size`. When the compressed size is smaller than the configured representation, it zero-fills the destination, copies the short prefix, and reports success. Unlike the compressed backends, it never validates the decoded representation length. Truncated/corrupt metadata can therefore produce an apparently valid image.

**Reproduce:** Probe `truncated_none` configures four `INT16` pixels, NONE differencing, BYTEPACK reordering, and NONE compression. Set `m_compressed_size=1` where the representation requires eight bytes. `xrif_decode` returns 0.

**Fix:** Validate the payload length against the configured representation before copying. Account explicitly for any supported padding convention; bytes needed for actual image data cannot be missing. Make the all-NONE shortcut enforce the same rule.

### F17 — Difference and reorder timings measure the opposite stages

**Location:** [src/xrif.c](../../../src/xrif.c), lines 1699–1711.

`xrif_reorder_time` subtracts difference-start from reorder-start, measuring differencing. `xrif_difference_time` subtracts reorder-start from compress-start, measuring reordering. The rate accessors and cached encode metrics inherit the swap.

**Reproduce:** Probe `performance` sets those three timestamps to 10, 11, and 14 seconds. Reported difference time is 3 seconds instead of 1, and reorder time is 1 instead of 3.

**Fix:** Swap the timestamp pairs to match the stage names and verify the corresponding rate/cache values.

## Remaining leads and limits

- OpenMP renibble encoding uses unsynchronized `+=` writes to `m_reordered_buffer2[pix/2]` and the matching high-nibble byte at `src/xrif_reorder_bytepack_renibble.c:204–205`. Adjacent pixels assigned to different threads can share those destinations. This is a code-review concurrency concern, but no corruption appeared in the tested 20,000-round-trip, 1,026-pixel run or the 200,000-round-trip, two-pixel run. It is not included in the reproduced finding count; investigate with a race detector and fix partitioning by output byte/pixel pair.
- FastLZ's bundled header requires at least 16 input bytes and at least 66 output bytes. `xrif_min_compressed_size_fastlz` does not enforce those minima. A sweep of input lengths 1–129 did not produce an ASan failure with this bundled implementation; no buffer-overflow claim is made from that mismatch alone. The matrix did expose UBSan alignment warnings in FastLZ's word-access optimizations, which merit a separate vendor portability review.
- Header loading into a new handle also needs explicit direction/context setup before zlib/Zstandard decoding. Calling `xrif_set_compress_direction(..., XRIF_DIRECTION_DECOMPRESS)` made those probe decodes succeed. Because direction selection is an existing explicit API concept, this was not counted as an independent defect in this audit.
- BITPACK is already marked broken in the README and was excluded from the added round-trip matrix. Bayer internals and bundled compression algorithms did not receive exhaustive independent validation. No big-endian, strict-alignment hardware, allocation-fault injection campaign, or long-running coverage-guided fuzzing was performed.

## Suggested repair order

1. Fix checked sizing, allocation ownership, and transform/decoder bounds (F01–F04); disable or repair dependent OpenMP pixel loops (F05).
2. Restore lossless output and consistent handle state (F06–F09, F12, F16), then repair library/test integration (F10–F11).
3. Correct API validation, portability, and metrics (F13–F15, F17); add regression cases from the probes and investigate the remaining concurrency lead.

## Reproduction appendix

Commands below assume the repository root as the working directory and normal zlib/Zstandard development packages. For the exact audit environment, add `-I/tmp/xrif-audit-deps/usr/include` to `CMAKE_C_FLAGS` and compiler commands, pass `-DLIBZSTD=/usr/lib/x86_64-linux-gnu/libzstd.so.1` to CMake, and replace `-lzstd` with `-l:libzstd.so.1`. Builds are intentionally outside the repository.

```sh
cmake -S . -B /tmp/xrif-audit-build -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/xrif-audit-build -j4
ctest --test-dir /tmp/xrif-audit-build --output-on-failure

cmake -S . -B /tmp/xrif-audit-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_FLAGS='-g -fsanitize=address,undefined -fno-omit-frame-pointer'
cmake --build /tmp/xrif-audit-asan --target xrif-static -j4
```

Save the C block below as `/tmp/xrif-audit-probe.c`, then compile both variants:

```sh
cc -g -O2 -I src /tmp/xrif-audit-probe.c \
  /tmp/xrif-audit-build/src/libxrif.a -fopenmp -lz -lzstd \
  -o /tmp/xrif-audit-probe
cc -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer \
  -I src /tmp/xrif-audit-probe.c /tmp/xrif-audit-asan/src/libxrif.a \
  -fopenmp -lz -lzstd -o /tmp/xrif-audit-probe-asan
```

Run non-crashing probes with `/tmp/xrif-audit-probe MODE`, using `matrix`, `header`, `overflow`, `omp_pixel`, `none`, `bytepack32`, `zlib_level`, `badconfig`, `performance`, or `truncated_none`. Run failing memory/UB probes separately with `ASAN_OPTIONS=detect_leaks=0 /tmp/xrif-audit-probe-asan MODE`, using `overflow crash`, `double_free`, `short_raw`, `short_unreorder`, `oversized_input`, `signed`, `null`, or `align`. Leak reporting is disabled because deliberately aborting probes cannot reach cleanup. A zero exit status alone is not evidence of correctness: inspect printed comparison values and UBSan diagnostics. `omp_nibble` and `fastlz N` exercise the additional leads, not counted confirmed failures.

For F10, save this minimal client as `/tmp/xrif-audit-link.c`:

```c
#include "xrif.h"
int main(void) {
    xrif_t x;
    if (xrif_new(&x)) return 1;
    return xrif_delete(x);
}
```

```sh
cc -I src /tmp/xrif-audit-link.c -L/tmp/xrif-audit-build/src \
  -lxrif -fopenmp -o /tmp/xrif-audit-link
readelf -d /tmp/xrif-audit-build/src/libxrif.so
```

### Probe source

```c
#include "xrif.h"
#include <stdint.h>
#include <limits.h>
#include <omp.h>

#define OK(call) do { int rv_ = (call); if(rv_) { fprintf(stderr, "%s = %d\n", #call, rv_); exit(2); } } while(0)

static xrif_t make(unsigned w, unsigned h, unsigned d, unsigned f, int type, int diff, int reorder, int comp, int onraw) {
    xrif_t x; OK(xrif_new(&x));
    x->m_compress_on_raw = onraw;
    OK(xrif_set_size(x, w,h,d,f,type));
    OK(xrif_configure(x,diff,reorder,comp));
    OK(xrif_allocate(x));
    memset(x->m_raw_buffer,0,x->m_raw_buffer_size);
    return x;
}
static uint32_t rng = 1234567;
static unsigned char byte(void) { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }

int main(int argc, char **argv) {
    if(argc < 2) return 2;
    setbuf(stdout,NULL);
    const char *mode=argv[1];
    if(!strcmp(mode,"header")) {
        int cs[] = {XRIF_COMPRESS_LZ4,XRIF_COMPRESS_ZLIB,XRIF_COMPRESS_ZSTD};
        for(int c=0;c<3;c++) {
            xrif_t a=make(4,3,1,2,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_PREVIOUS,XRIF_REORDER_BYTEPACK,cs[c],1), b;
            for(size_t i=0;i<a->m_raw_size;i++) a->m_raw_buffer[i]=byte();
            OK(xrif_encode(a));
            char header[48]; uint32_t hs;
            OK(xrif_write_header(header,a)); OK(xrif_new(&b)); OK(xrif_read_header(b,&hs,header));
            printf("codec=%d raw_size=%zu expected=%zu direction=%d\n",cs[c],b->m_raw_size,a->m_raw_size,b->m_compress_direction);
            OK(xrif_allocate(b)); memcpy(b->m_raw_buffer,a->m_raw_buffer,a->m_compressed_size);
            printf("header decode=%d\n",xrif_decode(b));
            OK(xrif_set_compress_direction(b,XRIF_DIRECTION_DECOMPRESS));
            memcpy(b->m_raw_buffer,a->m_raw_buffer,a->m_compressed_size);
            printf("after explicit direction decode=%d\n",xrif_decode(b));
            OK(xrif_delete(a)); OK(xrif_delete(b));
        }
    } else if(!strcmp(mode,"overflow")) {
        xrif_t x; OK(xrif_new(&x)); OK(xrif_set_size(x,65536,65537,1,1,XRIF_TYPECODE_INT16));
        printf("raw_size=%zu expected=%llu min_raw=%zu\n",x->m_raw_size,65536ULL*65537*2,xrif_min_raw_size(x));
        if(argc>2) { OK(xrif_allocate(x)); OK(xrif_difference(x)); }
        OK(xrif_delete(x));
    } else if(!strcmp(mode,"short_raw")) {
        xrif_t x=make(2,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_PREVIOUS,XRIF_REORDER_NONE,XRIF_COMPRESS_NONE,0);
        OK(xrif_set_size(x,4,1,1,1,XRIF_TYPECODE_INT16));
        printf("capacity=%zu required=%zu\n",x->m_raw_buffer_size,x->m_raw_size);
        printf("difference=%d\n",xrif_difference(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"omp_pixel")) {
        omp_set_dynamic(0); omp_set_num_threads(4);
        xrif_t x=make(4096,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_PIXEL0,XRIF_REORDER_NONE,XRIF_COMPRESS_NONE,1);
        int16_t original[4096]; for(int i=0;i<4096;i++) original[i]=i;
        for(int par=0;par<=1;par++) {
            x->m_omp_parallel=par; int bad=0;
            for(int trial=0;trial<100;trial++) { memcpy(x->m_raw_buffer,original,sizeof original); OK(xrif_difference(x)); OK(xrif_undifference(x)); if(memcmp(x->m_raw_buffer,original,sizeof original)) ++bad; }
            printf("parallel=%d corrupt_roundtrips=%d/100\n",par,bad);
        }
        OK(xrif_delete(x));
    } else if(!strcmp(mode,"omp_nibble")) {
        omp_set_dynamic(0); omp_set_num_threads(2);
        xrif_t x=make(1026,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_BYTEPACK_RENIBBLE,XRIF_COMPRESS_NONE,1);
        int16_t original[1026]; for(int i=0;i<1026;i++) original[i]=12345;
        for(int par=0;par<=1;par++) {
            x->m_omp_parallel=par; int bad=0;
            for(int trial=0;trial<20000;trial++) { memcpy(x->m_raw_buffer,original,sizeof original); OK(xrif_reorder(x)); OK(xrif_unreorder(x)); if(memcmp(x->m_raw_buffer,original,sizeof original)) ++bad; }
            printf("parallel=%d corrupt_roundtrips=%d/20000\n",par,bad);
        }
        OK(xrif_delete(x));
    } else if(!strcmp(mode,"none")) {
        xrif_t x=make(4,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_NONE,XRIF_COMPRESS_NONE,0);
        memset(x->m_raw_buffer,0x42,8); memset(x->m_compressed_buffer,0xCC,8);
        OK(xrif_encode(x)); printf("encoded_size=%zu compressed[0]=%02x expected=42\n",x->m_compressed_size,(unsigned char)x->m_compressed_buffer[0]);
        memset(x->m_compressed_buffer,0x42,8); memset(x->m_raw_buffer,0,8);
        OK(xrif_decode(x)); printf("decoded[0]=%02x expected=42\n",(unsigned char)x->m_raw_buffer[0]); OK(xrif_delete(x));
    } else if(!strcmp(mode,"bytepack32")) {
        xrif_t x=make(4,1,1,1,XRIF_TYPECODE_INT32,XRIF_DIFFERENCE_NONE,XRIF_REORDER_BYTEPACK,XRIF_COMPRESS_NONE,1);
        for(int i=0;i<16;i++) x->m_raw_buffer[i]=i;
        OK(xrif_reorder(x)); printf("reordered="); for(int i=0;i<16;i++) printf("%02x ",(unsigned char)x->m_reordered_buffer[i]); puts("");
        printf("unreorder=%d\n",xrif_unreorder(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"zlib_level")) {
        xrif_t x=make(32768,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_NONE,XRIF_COMPRESS_ZLIB,1);
        OK(xrif_set_zlib_level(x,0)); OK(xrif_set_zlib_strategy(x,Z_DEFAULT_STRATEGY));
        OK(xrif_encode(x)); printf("after setters level=%d strategy=%d compressed=%zu\n",x->m_zlib_level,x->m_zlib_strategy,x->m_compressed_size);
        OK(xrif_set_compress_method(x,XRIF_COMPRESS_ZLIB)); memset(x->m_raw_buffer,0,x->m_raw_size);
        OK(xrif_encode(x)); printf("after reinitialization compressed=%zu\n",x->m_compressed_size); OK(xrif_delete(x));
    } else if(!strcmp(mode,"double_free")) {
        xrif_t x=make(4,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_NONE,XRIF_COMPRESS_LZ4,0);
        OK(xrif_set_size(x,1073741824,1,1,1,XRIF_TYPECODE_INT16));
        printf("reallocate=%d\n",xrif_allocate_compressed(x));
        printf("delete=%d\n",xrif_delete(x));
    } else if(!strcmp(mode,"fastlz")) {
        unsigned n=argc>2?atoi(argv[2]):16;
        xrif_t x=make(n,1,1,1,XRIF_TYPECODE_UINT8,XRIF_DIFFERENCE_NONE,XRIF_REORDER_NONE,XRIF_COMPRESS_FASTLZ,1);
        for(unsigned i=0;i<n;i++) x->m_raw_buffer[i]=byte();
        printf("input=%u capacity=%zu\n",n,x->m_raw_buffer_size); printf("encode=%d\n",xrif_encode(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"signed")) {
        xrif_t x=make(2,1,1,1,XRIF_TYPECODE_INT32,XRIF_DIFFERENCE_PIXELL,XRIF_REORDER_NONE,XRIF_COMPRESS_NONE,1);
        ((int32_t *)x->m_raw_buffer)[0]=INT32_MIN; ((int32_t *)x->m_raw_buffer)[1]=INT32_MAX;
        OK(xrif_difference(x)); OK(xrif_undifference(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"badconfig")) {
        xrif_t x; OK(xrif_new(&x)); int rv=xrif_set_compress_method(x,123456);
        printf("invalid_method rv=%d method=%d\n",rv,x->m_compress_method);
        rv=xrif_set_compress_method_direction(x,XRIF_COMPRESS_LZ4,123456);
        printf("invalid_direction rv=%d direction=%d\n",rv,x->m_compress_direction); OK(xrif_delete(x));
    } else if(!strcmp(mode,"performance")) {
        xrif_t x; OK(xrif_new(&x));
        x->m_ts_difference_start=(struct timespec){10,0}; x->m_ts_reorder_start=(struct timespec){11,0}; x->m_ts_compress_start=(struct timespec){14,0};
        printf("difference=%g expected=1 reorder=%g expected=3\n",xrif_difference_time(x),xrif_reorder_time(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"align")) {
        xrif_t x=make(4,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_NONE,XRIF_COMPRESS_NONE,1);
        char *b=malloc(49); OK(xrif_write_header(b+1,x)); uint32_t hs; OK(xrif_read_header(x,&hs,b+1)); free(b); OK(xrif_delete(x));
    } else if(!strcmp(mode,"truncated_none")) {
        xrif_t x=make(4,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_BYTEPACK,XRIF_COMPRESS_NONE,1);
        x->m_compressed_size=1; x->m_raw_buffer[0]=0x42; printf("decode=%d with size 1 expected 8\n",xrif_decode(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"null")) {
        printf("set_direction(NULL)=%d\n",xrif_set_compress_direction(NULL,XRIF_DIRECTION_DECOMPRESS));
    } else if(!strcmp(mode,"oversized_input")) {
        xrif_t x=make(4,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_BYTEPACK,XRIF_COMPRESS_LZ4,1);
        memset(x->m_raw_buffer,0xff,x->m_raw_buffer_size); x->m_compressed_size=1000;
        printf("capacity=%zu advertised=%zu\n",x->m_raw_buffer_size,x->m_compressed_size);
        printf("decode=%d\n",xrif_decode(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"short_unreorder")) {
        xrif_t x=make(2,1,1,1,XRIF_TYPECODE_INT16,XRIF_DIFFERENCE_NONE,XRIF_REORDER_BYTEPACK,XRIF_COMPRESS_NONE,0);
        OK(xrif_set_size(x,4,1,1,1,XRIF_TYPECODE_INT16));
        OK(xrif_allocate_reordered(x)); memset(x->m_reordered_buffer,0,x->m_reordered_buffer_size);
        printf("capacity=%zu required=%zu\n",x->m_raw_buffer_size,x->m_raw_size);
        printf("unreorder=%d\n",xrif_unreorder(x)); OK(xrif_delete(x));
    } else if(!strcmp(mode,"matrix")) {
        int ds[]={-1,100,101,200,300,310}, rs[]={-1,100,200}, cs[]={-1,100,102,110,120,130};
        int checked=0;
        for(int w=1;w<=5;w++) for(int h=1;h<=3;h++) for(int d=1;d<=2;d++) for(int f=1;f<=3;f++)
        for(int di=0;di<6;di++) for(int ri=0;ri<3;ri++) for(int ci=0;ci<6;ci++) {
            xrif_t x=make(w,h,d,f,XRIF_TYPECODE_INT16,ds[di],rs[ri],cs[ci],1);
            char *original=malloc(x->m_raw_size); for(size_t i=0;i<x->m_raw_size;i++) original[i]=byte();
            memcpy(x->m_raw_buffer,original,x->m_raw_size); OK(xrif_encode(x));
            OK(xrif_set_compress_direction(x,XRIF_DIRECTION_DECOMPRESS)); OK(xrif_decode(x));
            if(memcmp(x->m_raw_buffer,original,x->m_raw_size)) { printf("FAIL %d,%d,%d,%d diff=%d reorder=%d comp=%d\n",w,h,d,f,ds[di],rs[ri],cs[ci]); return 1; }
            free(original); OK(xrif_delete(x)); checked++;
        }
        printf("serial INT16 roundtrips passed: %d\n",checked);
    } else return 2;
    return 0;
}
```
