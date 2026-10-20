# SPDX-License-Identifier: GPL-3.0-or-later

# Fails unless every source and build file in a git working tree begins with its SPDX line, naming
# each one that doesn't. The files are those git tracks, and those it would track: untracked files
# that aren't ignored count too.
#
#   GIT   the git program
#   ROOT  the working tree
#
# C++ files begin `// SPDX-License-Identifier: GPL-3.0-or-later`. CMake files, the Doxyfile and the
# clang-format and clang-tidy configurations begin with the same after `# `.

execute_process(
    COMMAND "${GIT}" ls-files --cached --others --exclude-standard
    WORKING_DIRECTORY "${ROOT}"
    OUTPUT_VARIABLE listing
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "git couldn't list the files in ${ROOT}.")
endif()
string(REPLACE "\n" ";" files "${listing}")

set(identifier "SPDX-License-Identifier: GPL-3.0-or-later")
set(missing "")
foreach(file IN LISTS files)
    cmake_path(GET file FILENAME name)
    if(name MATCHES "\\.(cpp|hpp)$")
        set(line "// ${identifier}")
    elseif(name MATCHES "^(CMakeLists\\.txt|.*\\.cmake|Doxyfile|\\.clang-format|\\.clang-tidy)$")
        set(line "# ${identifier}")
    else()
        continue()
    endif()
    # A file deleted from the working tree but not yet from the index has nothing to check.
    if(NOT EXISTS "${ROOT}/${file}")
        continue()
    endif()
    file(READ "${ROOT}/${file}" start LIMIT 256)
    string(FIND "${start}" "${line}\n" position)
    if(NOT position EQUAL 0)
        list(APPEND missing "${file}")
    endif()
endforeach()

if(missing)
    list(JOIN missing "\n  " names)
    message(FATAL_ERROR "These files don't begin with their SPDX line:\n  ${names}")
endif()
