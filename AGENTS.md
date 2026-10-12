# XRIF contributor and agent instructions

Follow these rules for project-owned files. The core library is C; C++ rules apply to the C++ interface and C++ tests only.

## 1. Plan documentation

- Document changes of significance (more than typos) in `agents/plans/YYYY-MM/<plan-name>.md`.
- Include the problem, solution and rationale, assumptions, implementation steps, edge cases, validation, and follow-up as relevant.
- Keep the plan current as engineering decisions or implementation details change.

## 2. Language scope and compatibility

- Keep the core library and its public C headers compatible with C11. Compile `.c` files as C, not as C++ to work around errors.
- Use C++17 as the initial minimum standard for the new C++ interface unless the task explicitly establishes another baseline. Declare that requirement on its CMake targets when adding the interface; do not change the C core's language standard.
- Keep C headers usable by both C and C++ consumers. Preserve `#ifdef __cplusplus` / `extern "C"` linkage guards, and place C++ classes, templates, and overloads outside them in separate C++ headers/sources.
- Keep the C core usable without the C++ interface or a C++ runtime dependency. Do not allow C++ exceptions to escape through a C API boundary.
- Treat exported names, signatures, public struct layout, constants, and serialized format values as compatibility contracts. Do not rename/reorder public fields or change ABI/format details solely for style or documentation organization. Explicitly document any intentional compatibility change and update relevant consumers and tests.

## 3. Style, reuse, and scope

- Follow the repository's coding conventions and the checked-in formatting configuration; favor concise, readable code.
- Look for existing solutions before introducing new implementations. Propose a shared utility when a recurring problem has not been generalized.
- Preserve behavior except for the requested fix or feature. Keep changes scoped and update dependent call sites when changing an API.
- Do not incidentally edit, reformat, or modernize bundled dependencies in `src/lz4/`, `src/fastlz/`, or `doc/doxygen-awesome-css/`.
- Do not hand-format generated lookup tables (`src/*.inc`) or generated build files. Change the appropriate generator when an output change is needed.

## 4. File-level documentation

- Project headers and sources should start with a Doxygen block containing `\file` and `\brief`.
- Do not add `\author` tags. Remove existing tags during a documentation pass, separately from an unrelated functional fix.
- Preserve copyright, license, and attribution notices; the Doxygen author-tag rule does not apply to those notices.

## 5. Includes and guards

- Match the existing include-guard naming convention.
- Preserve deliberate include ordering, feature-test macro placement, and conditional includes. Include sorting is disabled in `.clang-format`.
- Include the headers that declare the types/functions used rather than depending on incidental transitive includes. Use C-compatible includes in shared C headers.

## 6. Function documentation

- Give every public function declaration a brief `///` summary. Add a short `/** ... */` details block when needed.
- Document each public parameter inline at its declaration, preferably `type name /**< [in] description */`, using `[out]` or `[in/out]` as appropriate. Existing `///<` parameter comments are acceptable; use separate `\param` entries only when inline documentation is impractical.
- Document return values and errors for public APIs. For C interfaces, specify ownership, buffer lengths and units, nullability, allocation/release responsibilities, and the state left after failure where relevant.
- Document a private helper at its declaration, or at its definition if it has no separate declaration. A brief summary is sufficient for an obvious helper; explain non-obvious contracts without duplicating documentation at both sites.
- `///`, `///<`, and inline Doxygen block comments are valid in this C11 project and may also be used in C++.

## 7. Members and Doxygen sections

- Use the existing `m_` convention for struct/class fields; do not apply it to ordinary local variables or parameters. Keep public names/layout stable as described above.
- Document non-trivial fields with preceding `///` or trailing `///<` comments describing role, ownership, and state.
- Separate logical groups of declarations with blank lines; do not require a blank line between every related field.
- For C++ classes exposing configuration through member state and accessors, use `... - Data` sections for protected/private state and matching `...` sections for public accessors, placing the data section first when designing new declarations.
- For C structs, include `Data` in field-section names. C structs have no member functions or access-control sections; document their free accessor functions separately. Do not reorder existing struct fields to satisfy section ordering.

## 8. C++ definitions and resource lifetimes

- Keep non-trivial function definitions outside C++ class declarations, below the declaration or in `.cpp` files. Template definitions needed by callers must remain visible in headers or included implementation files.
- For a C++ block used only to control an RAII lock guard's lifetime, annotate its opening brace as `{ // mutex scope`.
- In C, braces do not release locks or allocations. Use explicit cleanup on every relevant exit path; a shared `goto cleanup` path is acceptable when it makes ownership and error handling clearer. Do not describe a C lexical block as providing automatic mutex cleanup.

## 9. Formatting

- Use clang-format 21 with the repository `.clang-format`; use `.editorconfig` for editor whitespace settings. Both use four-space indentation and LF endings for project C/C++ files. The formatter uses Allman braces and a 120-column limit.
- Format only the project-owned C/C++ source/header files in scope, respecting `.clang-format-ignore`. Do not pass Markdown, CMake, configuration files, bundled sources, generated tables, or build outputs to clang-format.
- For a newly formatted file, run `clang-format-21 -i <file>` and verify with `clang-format-21 --dry-run --Werror <file>`.
- For a legacy file, keep broad formatting cleanup separate from functional changes. Format changed regions for the functional patch, or put any required whole-file normalization in a separate formatting-only commit. Do not reformat the repository as a side effect of a small task.
- Check that formatting preserves Doxygen comments, preprocessor structure, and macro-based Check tests. The formatter's language settings do not choose the compiler's C/C++ standard.
- Report material style ambiguities and explain the chosen interpretation; routine choices within these rules do not need a separate approval step.

## 10. Build and verification

- Run the relevant build and tests for functional changes. Use a fresh or isolated CMake build when validating build-system changes, and distinguish existing failures or missing dependencies from regressions.
- For C API/header or library-linkage changes, compile and link small consumers with a C compiler in C11 mode and a C++ compiler in the supported minimum C++ mode against the C library. Check the supported static/shared linkage paths affected by the change.
- For C++ interface changes, also build and test that interface at its declared minimum standard, including ownership/error handling at the C boundary.
- For buffer sizing, ownership, or arithmetic changes, use relevant sanitizer and boundary-case checks. For parallel algorithm changes, test multiple worker threads and lossless round trips.
- For documentation/configuration-only changes, validate the affected configuration or documentation; do not run unrelated suites or add tests that merely mirror an edit.
- Report what was verified and any remaining verification limitations. A test registered but not run is not a passing test.

## 11. Test documentation

- Current C unit tests use Check (`START_TEST` / `END_TEST`). Preserve existing `\test`, `\anchor`, and `\ref` links between public API documentation and tests.
- If wrappers, macros, or test indirection obscure the API under test, add explicit Doxygen references to the real symbol in the test documentation. Do not introduce dependencies on another project's test harness or framework solely for documentation links.
- Hide harness-only helpers with `\cond ... \endcond` where appropriate. Do not insert non-compiling references into normal test compilation.
- Choose and document a C++ test framework when adding C++ tests; do not assume the current C tests use one.

## 12. Documentation and editing discipline

- Review documentation across a changed file for inaccuracies. Update contracts directly affected by the functional change in the same patch.
- Keep unrelated documentation cleanup, author-tag removal, and broad formatting out of the functional patch; perform them as separate passes/commits when in scope, or record follow-up work.
- Add new standing style/documentation instructions to this file as they are introduced, keeping C-only, C++-only, and shared rules clearly distinguished.

## 13. Branches and commits

- Feature branches must be named `<username>/<feature-name>`, for example `jrmales/gui-segfault-fixes`; use the active username.
- Do not commit directly to shared/integration branches, including `dev` and `main`. Create/switch to a feature branch before committing.
- Keep commits focused. Make functional changes with directly related API docs/tests first, include the relevant plan as part of that engineering record, then follow with unrelated documentation cleanup and a separate final formatting-only commit when needed.
- Keep commit messages brief, normally one summary line. Put additional engineering detail in the associated plan. Include `Co-authored by <model-name>` on an additional line, using the actual model name.

## 14. PR attribution

- At the top of PR descriptions for agent-assisted work, include `This work was performed by <current agent model name> in response to the prompt: "...".`
- Use the actual model name. If asked to execute a planning/task file, give its path; otherwise include the primary prompt verbatim, or a faithful condensed version if extremely long. A referenced planning file need not be summarized.
- When providing a PR description/title in chat, put the description in a copyable Markdown block and the suggested title in a separate copyable text block.

## 15. Completion report

- Summarize the changes and list affected files.
- State the validation performed and note follow-up items or relevant edge cases.
