# SPDX-License-Identifier: GPL-3.0-or-later

# Runs the SPDX check on a new git repository at DIRECTORY, which holds a C++ source and a CMake
# file that each begin with their SPDX line, and a README that needs none. BAD, if set, names one
# more file, whose SPDX line comes second instead of first.
#
#   GIT        the git program
#   DIRECTORY  where the repository is made, emptied first
#   BAD        the path of the badly headed file in the repository, if any

file(REMOVE_RECURSE "${DIRECTORY}")
file(MAKE_DIRECTORY "${DIRECTORY}")
execute_process(
    COMMAND "${GIT}" init --quiet
    WORKING_DIRECTORY "${DIRECTORY}"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "git couldn't make a repository in ${DIRECTORY}.")
endif()

set(identifier "SPDX-License-Identifier: GPL-3.0-or-later")
file(WRITE "${DIRECTORY}/src/thing.cpp" "// ${identifier}\n\nint thing;\n")
file(WRITE "${DIRECTORY}/CMakeLists.txt" "# ${identifier}\n\nproject(thing)\n")
file(WRITE "${DIRECTORY}/README.md" "# Thing\n")
if(DEFINED BAD)
    file(WRITE "${DIRECTORY}/${BAD}" "#pragma once\n// ${identifier}\n")
endif()

set(ROOT "${DIRECTORY}")
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/check_spdx.cmake")
