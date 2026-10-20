# Coding standards

How code in coldboot-emu is written. Where a tool can hold a rule, it does: `.clang-format`,
`.clang-tidy`, the warning flags in `cmake/ColdbootConventions.cmake` and the `Doxyfile` are the
record of those rules. This document covers the rest, and says why. Anything it leaves open follows
the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines); where it
departs from them, it says so.

## Language and build

- C++23, as ordinary headers rather than C++20 modules, with no compiler extensions, for baseline
  x86-64.
- Clang alone builds coldboot-emu, and its floor is Clang 22. The floor rises at milestones and
  never falls, and configuring with anything older, or with another compiler, stops with a message
  saying why. clang-format and clang-tidy are the floor's major exactly, since what they accept and
  how they format change between majors.
- CMake 4.4.2 or newer, with Ninja. Every supported configuration is a preset in
  `CMakePresets.json`, with a workflow preset, so that a developer and CI build alike.
- The build only finds its dependencies with `find_package`, and never downloads anything. The
  desktop tree takes them from the vcpkg manifest, with test-only ports in its `tests` feature.
- The core library compiles with `-ffreestanding`, so that a hosted header fails its build, and a
  test proves it does. That is a first fence, not proof: what the core may use is what builds and
  links in the UEFI image, which is libstdc++'s freestanding headers, such as `<array>`, `<span>`,
  `<optional>`, `<bit>` and `<algorithm>`. `<cstring>`, `<cassert>`, `<string_view>`, `<format>`,
  `<charconv>` and every hosted header are out. Members that throw, such as `optional::value()` and
  `array::at()`, don't link there either.
- The core and the frontends build with exceptions and RTTI off. Test programs keep both, since
  Catch2 stops a test by throwing.
- No namespace-scope object in the core or the UEFI frontend is dynamically initialised: each is
  `constexpr` or `constinit`, and `-Wglobal-constructors` is an error. Nothing runs constructors
  before the UEFI frontend's entry point.
- coldboot-emu is licensed GPL-3.0-or-later. Every source and build file begins with
  `// SPDX-License-Identifier: GPL-3.0-or-later`, or the same after `# ` in CMake files and tool
  configurations, with no copyright line. The `check-spdx` target checks it. JSON files, such as
  the presets and the vcpkg manifest, can't hold a comment, and are left out.
- Versions follow semantic versioning, and before 1.0 each milestone is a minor release. The version
  lives only in CMake's `project()`.

## Libraries

- The core library is one static library, `coldboot::core`, in `src/core/`. It holds the core, its
  components and the pure functions the frontends share with it. A frontend links it, and nothing
  else of the core's.
- Its headers sit under `src/core/include/coldboot/core/` and are included as
  `<coldboot/core/...>`. The core's interface is in `coldboot`. Its components are in
  `coldboot::detail`, with headers under `<coldboot/core/detail/>`: they are reachable from the
  public tree because the core holds each of them by value, as a fixed-size object must. A frontend
  never includes them. A test of a component may, to reach that component's interface.
- It is one library because a boundary inside it would hide nothing. The core's header needs the
  definition of every component it holds, the pure functions share the core's types and its
  freestanding rules, and every frontend that uses them links the core anyway. A component that has
  to be built or tested apart from the rest gets a library of its own, and a row in this table.
- A library links only what this table allows.

  | Library | Holds | Links |
  |---|---|---|
  | `coldboot::core` | the core and its components (the CPU and the bus), the stop report and its line, the text writer, and the assertions | nothing |
  | a frontend | its platform-free part and its platform files | `coldboot::core`, and its platform's libraries |
  | `coldboot_test_support` | what the tests share, in `tests/support/` | `coldboot::core`, Catch2 |
  | `coldboot_aborting_assertion`, `coldboot_precondition_main` | the two `assertion_failed` of the test programs | `coldboot_test_support` |

- The core is used from one thread at a time.

## Naming and spelling

- `PascalCase` for types, concepts, type aliases and template parameters.
- `snake_case` for functions, variables, parameters, namespaces, enumerators and `constexpr`
  constants, with no `k` prefix.
- Private and protected data members end in an underscore (`gpr_`). A plain struct's fields are
  bare.
- Enumerations are `enum class`.
- Macros only where nothing else will do, named `COLDBOOT_UPPER_CASE`.
- Everything is in one namespace, `coldboot`, with internals in `coldboot::detail`. The frontends
  are in `coldboot::uefi` and `coldboot::desktop`, and what the tests share in `coldboot::test`.
- Prose is British: comments, documentation, the README and commit messages. Every identifier is
  American, so a comment "initialises the colour" sits above `initialize_color`. One rule for every
  identifier means no word needs a judgement call.
- Hardware names are psx-spx's, lower-cased into `snake_case`: `gpustat`, `i_stat`, `i_mask`,
  `dicr`, `d2_madr`, and COP0's `sr`, `cause`, `epc` and `bad_vaddr`. CPU fields are the R3000
  manual's (`rs`, `rt`, `rd`, `hi`, `lo`), and mnemonics are lower case (`lwl`). A GP0 or GP1
  command's handler takes psx-spx's title for the command, with its code in the doc comment.
- The UEFI declarations keep the specification's spelling exactly (`EFI_SYSTEM_TABLE`, `ConOut`,
  `Blt`), in headers of their own, under one `NOLINTBEGIN(readability-identifier-naming)` giving
  the reason.
- Names from the project's domain use its terms, and never the words they replace:

  | Say | Not |
  |---|---|
  | core, the emulated PS1 | engine, system, PS1 part |
  | frontend | platform, platform part, host layer, shell |
  | UEFI frontend | bare-metal build, UEFI app, platform |
  | desktop frontend | host build, PC version |
  | BIOS, for the PS1's boot ROM image alone | firmware, boot ROM, PC BIOS |
  | Sony BIOS | real BIOS, retail BIOS, original BIOS |
  | OpenBIOS | open BIOS, replacement BIOS, free BIOS |
  | firmware, for the PC's UEFI firmware alone | BIOS, PC BIOS, CSM |
  | boot drive | USB stick, player's drive, boot disk, ESP |
  | SCE intro | boot logo, Sony logo, logo, splash |
  | PS logo | boot logo, licence screen, logo |
  | bootmenu | shell, BIOS menu, main menu |
  | emulated time, for the clock | cycle count, system time, core time, ticks |
  | video clock | region, console region, TV standard, video mode |
  | field | frame, scan, refresh |
  | VBlank | vsync, vertical retrace, blanking |
  | frame | vsync, refresh, picture |
  | displayed area | framebuffer, screen, display buffer, output |
  | guest | emulated program, ROM, software |
  | emulated | implemented, supported |
  | stubbed | dummy, mock, fake |
  | absent | unimplemented, unsupported, missing |
  | stop | halt, crash, exit, pause |
  | stop report | crash report, error, panic |
  | diagnostics | logs, debug output |
  | TTY text | console output, printf output, stdout |
  | POST code | boot code, status code |
  | device-access log | trace, I/O log, access trace |

  A duration may still be counted in cycles, but the clock is emulated time, so the core holds
  `now_`, never `cycles_`. A ROM is an image the core runs as its BIOS, never the guest: the BIOS
  ROM is the memory the core copies it into, and a test ROM is one built for a test.

## Files and formatting

- Files are `snake_case.hpp` and `snake_case.cpp`, with one main type per header, named after it.
  Headers use `#pragma once`.
- A library's public headers are included as `<coldboot/...>`, and a target's private headers as
  `"..."`.
- `clang-format` decides the layout, and groups includes as: the file's own header, this project's
  headers, third-party headers, then the standard library. Build the `format` target to apply it.

## The guest and the core's data

- Nothing the guest does is a bug in the core. Every value the guest controls is masked or wrapped
  as the hardware does before it indexes the core's memory, a shift by a guest amount is masked, and
  a guest division by zero gives psx-spx's result. The core's answer to the guest is emulated,
  stubbed or a stop, never an assertion and never undefined behaviour. A part the core doesn't build
  yet is absent too: meeting it is a stop, with a report that names it.
- Guest values are fixed-width unsigned (`std::uint32_t`, `std::uint16_t`, `std::uint8_t`), and
  guest arithmetic is unsigned, so it wraps as the R3000A does. A signed view is
  `static_cast<std::int32_t>`, and a guest value is truncated with a plain `static_cast`, as the
  hardware truncates it. The core's own values are narrowed only through a checked `narrow`, which
  comes with the first that needs it.
- A register that reads back what was written is held as that word, its fields read and written
  through named masks. No C++ bit-fields and no unions. A register whose read is computed builds its
  word in its read function.
- Guest memory (RAM, the BIOS ROM and the scratchpad) is bytes, read and written only through the
  load and store functions of `<coldboot/core/detail/guest_memory.hpp>`, built on `std::bit_cast`.
  They hold the one `static_assert` that the host is little-endian; a big-endian host is never
  supported. Nothing casts a pointer into guest memory, and no union reaches it. How VRAM is held is
  the rasteriser's.
- The core uses no floating point, so a run is the same on any host. The frontends may.

## Errors and assertions

- The core has no errors: creating it can't fail, and the guest is emulated, stubbed or a stop.
  `std::expected` appears only in the frontends.
- A frontend's failure that correct code can meet, such as a missing file, is returned as a value:
  the function returns `[[nodiscard]] std::expected<T, E>`. This departs from the Core Guidelines'
  E.2. `.value()` is never called, since it throws, and doesn't link in the UEFI image.
- Each failure domain has an error type of its own: an `enum class` code for programs to branch on,
  a fixed message for each code, and what the cause carries, such as the `EFI_STATUS` returned. No
  message is built where the failure happens, since the UEFI image has no heap; the desktop
  frontend's may also carry its library's text.
- Constructors never fail. Anything that can fail to come into being is made by a factory function.
- A broken precondition is a bug, not an error. `COLDBOOT_ASSERT(condition, message)` checks in
  every build, release included: the core's interface and the frontends.
  `COLDBOOT_DEBUG_ASSERT(condition, message)` checks inside the core where `NDEBUG` isn't defined,
  since the core's components sit on the per-instruction path. The message, a string literal, says
  what must hold, such as "a run ends later than it starts".
- A failed check calls `[[noreturn]] void coldboot::assertion_failed(const AssertionFailure&)`,
  with the condition, the message and a `std::source_location`. The core declares it, and every
  program that links the core defines it once: the UEFI frontend writes to debugcon and ConOut and
  halts, the desktop frontend writes to stderr and aborts, the Catch2 test programs do as the
  desktop frontend does, and the precondition tests report the failure and exit failing. Every
  target compiles with `-fmacro-prefix-map`, so recorded paths are relative to the repository.
- `noexcept` goes only where the standard library looks for it, on moves and `swap`. No `throw`,
  `try` or `catch` anywhere, tests included.

## Containment

- Inline assembly, compiler intrinsics, `volatile`, raw firmware pointers and `extern "C"` live only
  in the UEFI frontend's platform files: the files that talk to firmware or the CPU, such as port
  I/O, `rdtsc`, the `mem*` functions and the firmware tables. Each site has
  `NOLINT(<check>): <reason>`, never a relaxation for a directory. The core has none of them, and
  only what `<bit>` wraps.
- `-Wunsafe-buffer-usage` is on everywhere. Raw-pointer code sits only in those platform files,
  inside `#pragma clang unsafe_buffer_usage begin` and `end`, with a comment saying why.
- Text in the core and the UEFI frontend is `std::span<const char>`, with no terminating null;
  `literal()` makes one from a string literal. The core library's `TextWriter` writes decimal,
  hexadecimal and literal text into a fixed buffer, since `<charconv>`, `<format>` and
  `<string_view>` don't build in the image. The UEFI frontend widens text to `char16_t` for ConOut.
  Desktop-only code and the tests may use `std::string_view`, `std::format` and `std::print`.

## Ownership and types

- A plain struct that a caller fills in with a designated initialiser gives each member an
  initialiser, `{}` for a class, so that any member may be left out, unless it must be given, as a
  core's BIOS and diagnostics sink must.
- Values first, and `std::unique_ptr` for a single owner. No `std::shared_ptr` without a comment
  saying why, and no bare `new` or `delete`, except the UEFI frontend's placement new of the core.
- `T*` and `T&` never own. `T*` appears only where null means something.
- Firmware resources and SDL handles are owned by move-only RAII wrappers.
- The core is one fixed-size object that holds all of its memory and never allocates. It is never
  on the stack, which is too small for it, and it is neither copied nor moved.
- Counts and container indices are `std::size_t`, departing from the Core Guidelines' ES.107.
  Signed types are for arithmetic that can go negative.

## Warnings, clang-tidy and sanitizers

- The compiler warnings in `cmake/ColdbootConventions.cmake` are an explicit list, not
  `-Weverything`, so that a newer Clang builds the same tree. They are errors in every preset,
  through `CMAKE_COMPILE_WARNING_AS_ERROR`. Give a target of the core or a frontend its settings
  with `coldboot_target_conventions`, and a test program its own with
  `coldboot_target_test_conventions`. Third-party headers come in as system includes.
- clang-tidy runs the groups in `.clang-tidy`, with every warning an error, through the `tidy`
  preset. A finding that has to stand is suppressed with `NOLINT(<check>): <reason>` or
  `NOLINTNEXTLINE(<check>): <reason>`, never a bare `NOLINT`. Keep the reason on that line: any
  longer explanation goes in a comment above it.
- `_GLIBCXX_ASSERTIONS` is on in the debug and sanitize presets, and off in release.
- `desktop-sanitize` builds at `-O1` with AddressSanitizer and UndefinedBehaviorSanitizer, with no
  recovery, the core, the frontends and the tests instrumented and vcpkg's ports not. The UEFI
  image's debug build traps on undefined behaviour, needing no runtime. Never
  `-fsanitize=integer` or `implicit-conversion`, since guest arithmetic wraps by design, and never
  MemorySanitizer or ThreadSanitizer.

## Documentation

- Every declaration in every header has a `///` Doxygen comment, private members and `detail`
  included, and each header begins with a `\file` comment. Clang's `-Wdocumentation` checks each
  comment against its declaration, and the `docs` target runs Doxygen, which fails on any warning,
  an undocumented declaration among them.
- Behaviour from psx-spx names its section by heading; from a hardware test, the test and its log,
  such as ps1-tests' `cpu/access-time`. A figure from neither is marked `Guess:` where it is
  defined. Another emulator is never cited as the authority. Other emulators' code may be read to
  understand behaviour, never copied or translated.
- A comment says why, not what.

## Tests

- Tests come first: a test that fails, then the code that passes it. A new test that passes at once
  is checked by breaking the code it covers, since a test that can't fail proves nothing.
- A test exercises the core as a whole or one component's interface, and survives any refactoring
  that keeps the behaviour. There are no test-only accessors, `friend` tests or
  `#define private public`. Its name is a sentence saying what holds, such as "a stop is final:
  every later run returns the same report at once".
- `tests/` mirrors `src/`, with a file's tests in `<name>_tests.cpp`. A component is tested
  through the core's interface, and through its own only where the core's is too blunt, so the
  CPU and the bus have no test files of their own yet. Beside them, `tests/cmake/` tests the
  build's own checks, `tests/roms/` writes the test ROMs to files, and `tests/support/` holds what
  the tests share: the `constexpr` R3000A encoder, the ROM builder and the test ROMs, a
  `Diagnostics` that records what it receives, and Catch2's printing of the core's values.
- Unit tests build their guest input in C++, through the encoder and the ROM builder, need no MIPS
  toolchain and never read files. The encoder's own tests compare it with LLVM's encodings, so
  that it and the core's decoding can't agree on a mistake. Each component's tests keep hand-picked
  hostile guest values.
- The `test-roms` target writes each test ROM to `test-roms/` in the build tree, for the frontends
  and the tools that boot a ROM from a file. A unit test runs the same ROM from memory.
- Every test carries exactly one CTest label, naming what it needs: so far only `unit`, the CPU
  alone and no files. Its name begins with the component it tests, matching its Catch2 tag
  (`core: ...`, `[core]`), or with `build` for the build's own checks. A test whose inputs aren't
  configured is registered disabled, so that CTest lists it as not run.
- Each kind of test has a helper in `tests/CMakeLists.txt`:
  - `coldboot_add_tests` adds a component's Catch2 test cases, each registered with CTest. A test
    case's name must not begin with `-`, which Catch2 would read as one of its options.
  - `coldboot_add_precondition_test` adds a program that breaks one precondition. It passes only
    on the report of that precondition's check, so that another check firing first fails it, and
    the program exits rather than aborting, since CTest counts an abort as a crash. These are used
    sparingly.
- A test file whose checks hold one of the core's values includes `core_printing.hpp` from
  `tests/support/`. One that didn't would print the value as `{?}`, and its copy of Catch2's
  printing could replace the right one in every other file of the program.

## Commits and branches

- Commit subjects follow Conventional Commits, `type(scope)!: description`, with the type one of
  `build`, `chore`, `ci`, `docs`, `feat`, `fix`, `perf`, `refactor`, `revert`, `style` or `test`.
  The scope and the `!` of a breaking change are optional. A message describes the change and why.
- Branches follow git flow. `main` holds released versions only, and each merge into it is tagged
  with its version. `develop` is where work comes together. A change is made on `feature/<slug>`
  from `develop`, its slug naming the change; a release on `release/<version>`; and a fix to a
  release on `hotfix/<version>` from `main`. Every merge is `--no-ff`.
- Every branch gets a pull request, which must pass the `ci` check before the branch is finished
  with `git flow`.
