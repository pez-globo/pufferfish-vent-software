# arm-none-eabi CMake toolchain file

# This CMake toolchain file configures the build for the embedded firmware which
# runs on ARM. It is customized for the Pufferfish build scripts.

# Copyright (c) 2020 Pez-Globo and the Pufferfish project contributors
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied.
#
# See the License for the specific language governing permissions and
# limitations under the License.

# This file is adapted from jobroe's toolchain-arm-none-eabi.cmake file from
# https://github.com/jobroe/cmake-arm-embedded/, but with heavy modifications
# to customize it for the Pufferfish build scripts.
# Third-party works included in this file are licensed as follows:
#
# MIT License

# Copyright (c) 2018 Johannes Bruder

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# make modules visible
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# set target definition
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# set toolchain paths
set(TOOLCHAIN arm-none-eabi)
if(NOT DEFINED TOOLCHAIN_PREFIX)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
        set(TOOLCHAIN_PREFIX "/usr")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
        set(TOOLCHAIN_PREFIX "/usr/local")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "Please specify the TOOLCHAIN_PREFIX !\n For example: -DTOOLCHAIN_PREFIX=\"C:/Program Files/GNU Tools ARM Embedded\" ")
    else()
        set(TOOLCHAIN_PREFIX "/usr")
        message(STATUS "No TOOLCHAIN_PREFIX specified, using default: " ${TOOLCHAIN_PREFIX})
    endif()
endif()
if (
    DEFINED TOOLCHAIN_BIN_DIR AND
    NOT TOOLCHAIN_BIN_DIR STREQUAL "" AND
    NOT TOOLCHAIN_BIN_DIR STREQUAL "${TOOLCHAIN_PREFIX}/bin"
)
    message(STATUS "Overriding TOOLCHAIN_BIN_DIR: " ${TOOLCHAIN_BIN_DIR})
else()
    set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
endif()
set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/include)
message(STATUS "Toolchain includes should be in: " ${TOOLCHAIN_INC_DIR})
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/lib)
if(WIN32)
    set(TOOLCHAIN_EXT ".exe" )
else()
    set(TOOLCHAIN_EXT "" )
endif()

# specify cross compilers and tools
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-g++${TOOLCHAIN_EXT})
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT})
set(CMAKE_AR ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objdump)
set(SIZE ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# use hardware fpu
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv5-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv5-sp-d16)

# set clang options
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Clang")
    set(CMAKE_SYSROOT ${TOOLCHAIN_PREFIX}/${TOOLCHAIN})
    find_path(SYSTEM_CXX_PATH cmath HINTS ${TOOLCHAIN_INC_DIR}/c++/*)
    add_compile_options(  # Clang options
        --sysroot=${TOOLCHAIN_PREFIX}/${TOOLCHAIN}
    )
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -isystem${SYSTEM_CXX_PATH} -isystem${SYSTEM_CXX_PATH}/${TOOLCHAIN}"
    )
endif ()

add_compile_options(-mcpu=cortex-m7 -mthumb)
add_compile_options(--specs=nano.specs)
add_compile_options(
    -ffunction-sections -fdata-sections
    -fno-exceptions
    -fno-common -fmessage-length=0
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-threadsafe-statics")
add_compile_options(-Wall)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(--specs=nano.specs)
add_link_options(-mcpu=cortex-m7 -mthumb)
