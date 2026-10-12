# F01: checked 64-bit sizes and version-1 headers

Date: 2026-10-12

Status: **Implemented and verified.**

Basis: [F01 in the bug audit](2026-10-11-xrif-bug-audit.md#f01--dimension-multiplication-wraps-in-32-bits). Guidance/formatting and the historical audit were committed separately as `ce30a52` and `0f128e3`.

## Problem and authorized scope

`xrif_dimension_t` is `uint32_t`. Expressions such as `w*h*d*f*sz` multiplied dimensions in 32-bit arithmetic before reaching the final `size_t` operand. The audit's `65536 × 65537 × 1 × 1` INT16 shape needs **8,590,065,664 bytes**, but previously reported 131,072 and allocated only 131,602 raw bytes with default LZ4. Related narrow arithmetic occurred in buffer bounds, transform offsets, and loop counters.

The original F01 plan preserved the version-0 header and proposed rejecting compressed sizes above `UINT32_MAX`. The user expanded the scope: raise compressed payload lengths to `UINT64_MAX`, retain individual 32-bit axes, support checked larger products throughout the library, clearly document both header formats, and publish on `dev`. That instruction authorizes this task's `dev` update despite the default branch guidance in `AGENTS.md`. Work is prepared on `jrmales/audit-and-sizing-plan` before a normal fast-forward push.

## Decisions and compatibility

- Preserve the C11 public struct layout, existing signatures, type codes, and method identifiers. Existing `size_t` size fields support the full uint64_t range on the target 64-bit host. Arithmetic rejects values exceeding either `UINT64_MAX` or host `SIZE_MAX`; 32-bit hosts cannot process unaddressable images.
- Keep `xrif_set_size` codec-independent and state-preserving on failure. Valid wide geometry can subsequently be rejected by a specific codec's limits. The original F01 shape is accepted exactly on a capable host; default LZ4 allocation rejects it before allocating any image buffer.
- Emit **version 1**, with a **56-byte little-endian header**: common fields through byte 35, reserved bytes 36–39, uint64_t payload length at 40–47, codec options at 48–55. Payload length excludes the header. Header length remains uint32_t.
- Continue reading **version 0**, whose 48-byte header stores a native-endian uint32_t payload length at 36–39 and options at 40–47. Do not reinterpret legacy endianness or silently emit a new layout under version 0. Old libraries reject version 1 by version number.
- Add capacity-aware `xrif_write_header_sized` and `xrif_read_header_sized`. Retain existing wrappers with their documented caller-capacity requirements. Callers must recompile and replace literal 48-byte writer allocations with `XRIF_HEADER_SIZE`.
- Header serialization uses byte operations / memcpy, avoiding alignment assumptions. Reading validates in staged state, reconstructs raw size, and leaves handle/output unchanged on failure. It preserves existing buffers and codec context/direction; callers configure decompression explicitly.
- Preserve transform payload layouts, including existing renibble padding. BITPACK and independently broken type/parallel paths remain outside the supported round-trip claims. No C++ interface or chunked LZ4/FastLZ container is added.

The full normative layout, signed method/type tables, codec option offsets, extension behavior, byte order, host limits, and migration instructions are in [doc/header-format.md](../../../doc/header-format.md), linked from README and included in Doxygen input.

## Implementation

### Shared arithmetic and storage

`src/xrif_size_internal.h` supplies private static-inline checked multiplication/addition, derived geometry in pixels/bytes, method extents, and buffer preflight. All products are widened before evaluation and checked before multiplication; frames + 1 and padding additions are checked separately. No private helper is exported or installed.

Geometry contains element bytes, plane/frame/total pixels, and frame/raw bytes. Operation boundaries recompute it and validate cached sizes because the handle is public. Transform capacity checks use bytes. Typed entry points also check element width before treating storage as that type.

`xrif_set_size` assigns metadata only after all geometry checks pass. Minimum-size APIs return zero on invalid metadata, overflow, or an unsupported codec size. The compression-on-raw bound no longer hides a zero codec bound with a raw-size fallback. `xrif_allocate` checks all required method extents before allocating anything, including separate compressed storage.

Buffer setters validate before releasing owned data. Individual allocators allocate replacement storage before freeing the previous buffer, retaining it on failure. Passing the existing pointer to a setter retains its ownership. A later malloc failure during multi-buffer allocation can leave partial progress, but storage remains safely resettable/deletable; full malloc rollback is not promised.

### Transforms and NONE paths

Active difference/reorder entry points validate geometry and storage before pointer arithmetic. Plane/frame products use shared checked geometry. Dimension/pixel traversal counters use `size_t`; offsets are bounded by validated geometry. Renibble half-count rounding is exact integer ceiling division. The active Bayer INT16 routine rejects widths below two or odd widths before subtractive indexing; disabled experimental blocks are unchanged.

NONE reordering validates both buffers before copying. NONE compression uses the checked representation extent rather than arbitrary backing-buffer capacity, and its decoder requires the expected complete length. The all-NONE shortcut now validates and copies separate compressed storage in both directions, retaining a zero-copy path when compression is on raw.

### Codec interfaces

| Codec | Integration |
| --- | --- |
| LZ4/LZ4HC | Check bundled `LZ4_MAX_INPUT_SIZE` before bounds/narrowing. Pass a bounded destination capacity fitting int. Decompression rejects nonrepresentable signed lengths before the backend call. |
| FastLZ | Integer output bound of input + floor(input/20) + 1, at least 66 bytes; input and bound must fit int. Tiny inputs remain supported with the same one-literal-run wire encoding, generated in XRIF to avoid calling FastLZ below its documented 16-byte minimum. Both levels have byte-level regression tests. |
| zlib | Guard uLong bound calculations and overflow. Keep size_t stream totals, feeding input/output windows no larger than UINT_MAX. This remains one zlib stream with no new container framing. A separate test target forces 17-byte windows to exercise transitions using small allocations. |
| Zstandard | Check error-encoded `ZSTD_compressBound` results, retain its native size_t interface, and reject data-call extents beyond PTRDIFF_MAX. |
| NONE | Copy the checked representation length with no codec-imposed 32-bit ceiling. |

Every decompressor compares the advertised compressed length with its actual source capacity before reading it. Oversized backing capacities are never blindly narrowed. Data produced by zlib is counted from each offered window instead of potentially wrapping library total counters.

## Error and state contracts

| Condition | Result |
| --- | --- |
| Zero dimension or unrepresentable geometry | `XRIF_ERROR_INVALID_SIZE`, sizing state unchanged |
| Invalid type | `XRIF_ERROR_INVALID_TYPE`, sizing state unchanged |
| Missing setup / buffer | Existing `XRIF_ERROR_NOT_SETUP` handling |
| Invalid/unrepresentable size query | 0 |
| Unsupported method size | `XRIF_ERROR_INVALID_SIZE` before backend data access |
| Valid extent but inadequate capacity | `XRIF_ERROR_INSUFFICIENT_SIZE` |
| Individual replacement malloc failure | `XRIF_ERROR_MALLOC`, prior storage retained |
| Header truncation | `XRIF_ERROR_INSUFFICIENT_SIZE`, handle/output unchanged |
| Bad magic or too-short declared header length | `XRIF_ERROR_BADHEADER` |
| Future header version | `XRIF_ERROR_WRONGVERSION` |
| Header uint64_t length exceeding host SIZE_MAX | `XRIF_ERROR_INVALID_SIZE` |

A raw/payload extent can fit uint64_t while padding, a codec bound, or a caller's combined header-plus-payload allocation cannot. Each calculation must be checked independently; no global 32-bit limit is imposed.

## Regression coverage

`tests/xrif_test_sizes.c` and its small-window zlib variant cover:

- The original F01 shape, wrap-to-zero products across different axes, uint64_t overflow, and the exactly representable UINT64_MAX shape `(UINT32_MAX, 641, 6700417, 1)` with UINT8 elements.
- Checked padding with `frames=UINT32_MAX`, LZ4 boundary and boundary+1, FastLZ int/bound limits, wide zlib/Zstandard queries and bound overflow.
- Setter/allocator rejection over owned storage followed by safe deletion; enlarged geometry with intentionally small buffers across active transform dispatchers.
- Exact version-1 golden bytes, unaligned buffers, negative method/Zstandard fields, payload lengths at UINT32_MAX, UINT32_MAX+1, and UINT64_MAX, and native-endian version-0 reading.
- All truncated version-1 lengths, invalid versions/declared lengths/overflowing geometry, no output mutation on errors, and declared header extensions.
- All six codecs with both buffer modes over small sizes, separate all-NONE storage, tiny FastLZ literal compatibility, advertised source lengths beyond capacity, and header-driven decoding with fresh handles.

Existing initialization header assertions now inspect little-endian bytes at version-1 offsets without typed unaligned loads; the zlib header test is corrected and registered. New size targets are registered without rewriting the unrelated stale CTest entry.

Large boundaries are exercised through metadata and queries, never multi-gigabyte allocation or traversal. Real large allocations and big-endian/32-bit execution are not implied by these tests.

## Verification and remaining work

Validation completed on Linux x86-64 with GCC 15.2, C11, clang-format 21, and the existing installed zlib/Zstandard runtimes. The missing Zstandard development header was supplied from the prior audit's extracted package under `/tmp/xrif-audit-deps`; no system installation was changed.

- Fresh normal build at `/tmp/xrif-wide-build`: succeeded. Full CTest: **7 passed, 1 not run** (the pre-existing nonexistent pixel executable, F11). Existing increment and white-noise suites pass. The three omitted previous0/pixel0/pixelL difference suites each passed all six checks when run directly.
- Both new boundary executables passed **11 checks each**, including four-worker OpenMP comparisons on PREVIOUS0/PREVIOUS/FIRST/PIXELL with BYTEPACK. Known dependent PIXEL0 and renibble parallel paths were excluded from multi-worker claims.
- The full serial INT16 matrix passed **9,720 round trips** across widths 1–5, heights 1–3, depths 1–2, frames 1–3, six difference settings, three supported reorder settings, and all six codecs. Tiny FastLZ inputs retain success and their literal encoding.
- ASan/UBSan builds at `/tmp/xrif-wide-asan`: both boundary suites passed all 11 checks and the full 9,720-case matrix completed with no AddressSanitizer finding. UBSan reports were confined to the previously known misaligned accesses in bundled `src/fastlz/fastlz.c`; this is not a clean UBSan run. LeakSanitizer cannot run under this environment's tracing, so final runs used `ASAN_OPTIONS=detect_leaks=0`; leak coverage is not claimed.
- OpenMP-disabled build at `/tmp/xrif-wide-serial`: initialization and both boundary targets passed. Its existing ASCII test requires a POSIX feature macro without OpenMP's implicit feature definitions; supplying `-D_POSIX_C_SOURCE=200809L` allowed the full build. This verification workaround did not change the unrelated ASCII source.
- Strict C11 and C++17 consumers (`-Wall -Wextra -Werror -pedantic`) compiled, linked, and ran against both static and shared libraries. They round-tripped a version-1 header with wide raw geometry and `UINT64_MAX` payload metadata. Existing F10 requires explicit zlib/Zstandard/OpenMP dependency linking. All prior dynamic symbols remain, with only the two sized header APIs added.
- Doxygen completed successfully under `/tmp/xrif-wide-doc`; the generated header-format page contains both layout tables, the uint64_t maximum, option offsets, and migration/API guidance. Existing documentation/configuration warnings remain separate. README and Doxygen both link to this specification.
- New C/header files pass clang-format 21's dry-run check. Legacy files were formatted only around changed regions; `git diff --check` passes. Reviewed remaining active dimension products, subtractive strides, and codec length casts; surviving raw dimension expressions are in disabled Bayer code or are bounded by checked geometry.

No multi-gigabyte buffers were allocated for boundary tests. Actual large-buffer throughput, big-endian execution, and allocation-fault injection are not claimed.

A real 32-bit compile was attempted with `cc -m32`; this host lacks the 32-bit libc headers (`bits/libc-header-start.h`). Architecture-conditional tests are present, but actual 32-bit execution remains a coverage gap.

Known unrelated audit findings are tracked separately. In particular, F05's OpenMP dependency races, F08's 32-bit BYTEPACK implementation, F09's zlib parameter propagation, F10's library dependency linkage, F11's stale CTest registration, F12's signed pixel arithmetic, F15's configuration reporting, and F17's timing labels are not represented as fixed. Bundled FastLZ alignment diagnostics and BITPACK behavior remain independent limitations.

## Overlap with other findings

The full operation-path fix necessarily also addresses the audited F02 allocation failure, F03 byte-capacity guards, F04 compressed-source bounds, F06 separate-buffer NONE shortcut, F07 header raw-size reconstruction, F14 unaligned headers, and F16 incomplete NONE payloads. Touched null-handle paths improve part of F13, but other configuration APIs remain outside this change. Resolution claims will be tied to the actual regression results in the historical audit's follow-up notes.
