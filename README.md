# coldboot-emu

coldboot-emu is a PlayStation 1 emulator meant to be the only software on an x86-64 PC: the PC's
UEFI firmware starts it from a USB drive, and it runs PS1 games with no operating system underneath.

It is at an early stage. Today it holds the core library, `coldboot::core`, whose core runs a BIOS
from the reset vector until it meets a part of the PS1 it doesn't build, then stops for good and
reports what it touched, where, by which instruction and when. Its CPU decodes SLL, LUI, ORI, ADDIU
and SW so far, and its bus maps the BIOS ROM and 2 MiB of RAM.

## Requirements

- Clang 22 or newer, with libstdc++. Configuring with anything older, or with another compiler,
  stops with a message saying why.
- CMake 4.4.2 or newer, and Ninja.
- [vcpkg](https://vcpkg.io), with `VCPKG_ROOT` naming a clone of it, for the tests' Catch2.
- For the checks: clang-format and clang-tidy at Clang 22, Doxygen and git.

## Building

Every supported configuration is a preset in `CMakePresets.json`, and a workflow preset configures,
builds and tests in one step:

```sh
cmake --workflow --preset desktop-debug
```

| Preset | Configuration |
|---|---|
| `desktop-debug` | debug, with libstdc++'s assertions |
| `desktop-release` | release |
| `desktop-sanitize` | AddressSanitizer and UndefinedBehaviorSanitizer, at `-O1` |
| `tidy` | debug, with clang-tidy run on every source |
| `lint` | the formatting, the documentation and every file's SPDX line, building nothing else |

A build also writes the ROMs that the tests build in C++ to `test-roms/` in its tree, through the
`test-roms` target, for anything that boots a ROM from a file.

## Licence

coldboot-emu is licensed under the GNU General Public License, version 3 or later: see `LICENSE`.
How its code is written is in `CODING_STANDARDS.md`.
