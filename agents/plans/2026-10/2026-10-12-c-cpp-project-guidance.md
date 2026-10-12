# C/C++ project guidance and formatting — 2026-10-12

## Request and scope

Review the supplied `.clang-format` and `.editorconfig` for C compatibility, and update `AGENTS.md` using the agreed review recommendations while retaining guidance for the planned C++ interface.

Both formatting files were already present as untracked files when this work started. Preserve their four-space indentation, Allman braces, 120-column limit, deliberate include ordering, and Markdown/Makefile intentions. Much of the existing C source uses three spaces; this change establishes the supplied four-space convention for new or deliberately formatted code without applying a repository-wide rewrite.

Only repository guidance and formatting configuration are in scope. No library implementation, build-system implementation, existing test source, audit findings, or benchmark files are to change.

## Decisions and C review

- Establish clang-format 21 as the formatting baseline, with explicit LLVM fallback settings and a shared C/C++ default section. Retain the template declaration preference as a C++ override. Formatter language selection is distinct from compiler language standards; verify actual C syntax after formatting. The version-specific [Clang formatting options](https://releases.llvm.org/21.1.0/tools/clang/docs/ClangFormatStyleOptions.html) describe language sections and the current option names.
- Replace deprecated option spellings with the corresponding clang-format 21 forms, set LF/final-newline behavior consistently with EditorConfig, preserve include order and Doxygen/license text, and avoid indenting the C API merely because it sits within `extern "C"`. Teach the formatter that Check's `END_TEST` can stand alone without a semicolon.
- Add `.clang-format-ignore` so normal file-based formatting skips bundled sources, generated lookup tables, and build trees, using the documented [ignore-file mechanism](https://releases.llvm.org/21.1.0/tools/clang/docs/ClangFormat.html#clang-format-ignore). Editor integrations that format stdin may still need explicit exclusion settings; the contributor instructions forbid incidental formatting of these paths regardless of editor behavior.
- Keep UTF-8, LF, four spaces, and final-newline settings for project files. Keep Markdown trailing spaces, extend Makefile tab rules to common makefile names and `.mk` files, and remove the nonstandard Python `quote_type` setting. EditorConfig does not choose C/C++ syntax or standards. Unset normalization properties for bundled/generated paths; this relinquishes project settings rather than guaranteeing editor defaults will preserve a file. See the [EditorConfig property and glob specification](https://spec.editorconfig.org/).
- Preserve C11 for the core. Set C++17 as the initial interface baseline unless a subsequent task explicitly changes it; future interface CMake targets must declare this themselves. No C++ target or build-standard change is introduced here.
- Preserve the public C API, ABI, and serialized representation. Keep the C core usable independently of C++ and prevent exceptions from crossing C boundaries.
- Scope class definitions and RAII lock-lifetime comments to C++. Explicit C cleanup, including a shared cleanup label, remains appropriate. C11 supports the repository's existing line comments and Doxygen conventions.
- Consolidate parameter documentation rules, require relevant public C ownership/error contracts, and replace MagAO-X/Catch-specific guidance with current Check and Doxygen conventions. Retain template visibility guidance for the future interface.
- Reconcile minimal functional patches with documentation/formatting passes. Keep broad cleanup separate, preserve legal notices, and specify build, C/C++ consumer, sanitizer, and concurrency validation when relevant.

## Implementation and validation

1. Update the supplied formatting files and add formatter exclusions.
2. Rewrite and renumber `AGENTS.md`, retaining branch, commit, attribution, planning, and completion conventions.
3. Parse the format configuration for C and C++ inputs; check formatting idempotence on representative C11 and C++17 snippets, public-header guards, and Check macros.
4. Resolve EditorConfig properties with the installed EditorConfig core for C/C++ sources, Markdown, makefiles, bundled files, generated tables, and build paths.
5. Compile representative formatted C/C++ snippets and consumer smoke programs without changing production code; verify only the intended configuration/documentation files changed.

Validation completed with clang-format 21.1.8, GCC 15.2, and the installed `libeditorconfig` C implementation:

- The formatter parsed the configuration for `.c`, `.h`, `.cpp`, and `.hpp` inputs. Resolved indentation, tab policy, line length, line endings, final newline, and include-order settings matched the intended values.
- Representative formatted C11 and C++17 programs compiled with `-Wall -Wextra -Werror -pedantic-errors` and ran successfully. The C sample covered designated initializers, compound literals, `restrict`, `_Generic`, `_Static_assert`, and a statement macro; the C++ sample covered an out-of-class template definition and scoped mutex ownership. Formatting each twice produced identical output.
- A formatted temporary copy of `src/xrif.h` preserved include order. C11 and C++17 consumers compiled, linked, and ran using both the static and shared C libraries from the previous audit build. Explicit `-fopenmp -lz -l:libzstd.so.1` flags worked around the already documented library dependency issue. The temporary Zstandard header package from that audit supplied the otherwise missing development header.
- A formatted temporary copy of the actual `tests/xrif_test_init.c` compiled successfully, including its Check macros and Doxygen comments, and was stable on a second formatting pass.
- EditorConfig properties resolved correctly for C/C++ sources and headers, Markdown, all listed makefile names, `.mk` files, bundled paths, generated tables, and root/nested build directories. An initial glob missed root build directories; adding an explicit root pattern corrected it. A dedicated `.clang-format` section preserves that YAML file's two-space nesting while C/C++ stays at four spaces.
- In a temporary fixture tree, `clang-format-21 --dry-run --Werror` skipped all excluded paths and correctly reported an unformatted project-owned C file. No excluded repository file was reformatted.
- Checks and smoke programs were kept under `/tmp/xrif-style-review`, not added to the repository. A full library test-suite rerun was unnecessary for these configuration/documentation edits; existing audit failures are not claimed fixed.

Affected files: `.clang-format`, `.clang-format-ignore`, `.editorconfig`, `AGENTS.md`, and this plan. Production sources and the prior audit report remain unchanged.

## Follow-up

- Implementing the C++ interface will require actual CMake standard declarations, ownership/error-policy decisions, and a test-framework choice.
- Existing audit findings, including incomplete shared-library dependency linkage and the stale CTest executable registration, remain separate work.
- Broader normalization of existing source is deliberately deferred to a formatting-only change.
