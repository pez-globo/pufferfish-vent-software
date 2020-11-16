cmake_minimum_required(VERSION 3.16)

# project settings
project(ventilator-controller-stm32 C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# add_compile_options(-v)  # verbose outputs, useful to troubleshoot clang-tidy

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Clang")
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

include_directories(SYSTEM
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS/Include
)
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/Core/Inc
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "TestCatch2")
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    # disable all optimization
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-inline -fno-inline-small-functions -fno-default-inline")

    setup_target_for_coverage_lcov(
        NAME ${CMAKE_BUILD_TYPE}_coverage
        EXECUTABLE ${CMAKE_BUILD_TYPE}
        EXCLUDE "/usr/include/*" "Core/Inc/catch2/*" "Core/Test/*" "Core/Src/nanopb/*"
    )

    file(
        GLOB_RECURSE LIBRARY_SOURCES
        "Core/Src/Pufferfish/Driver/Indicators/PulseGenerator.cpp"
        "Core/Src/Pufferfish/Driver/Serial/*.*"
        "Core/Src/Pufferfish/Application/*.*"
        "Core/Src/Pufferfish/Util/*.*"
        "Core/Src/Pufferfish/HAL/CRC.cpp"
        "Core/Src/Pufferfish/HAL/Mock/*.cpp"
        "Core/Src/nanopb/*.c"
    )
    add_library(Pufferfish ${LIBRARY_SOURCES})

    file(GLOB_RECURSE EXECUTABLE_SOURCES "Core/Test/*.*")

    add_executable(${CMAKE_BUILD_TYPE} ${EXECUTABLE_SOURCES})
    include_directories("Core/Inc")
    include_directories("Core/Test/Inc")
    target_link_libraries(${CMAKE_BUILD_TYPE} Pufferfish gcov)
else ()
    add_definitions(-DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG)

    file(GLOB_RECURSE SOURCES "Core/Src/*.*" "Drivers/STM32H7xx_HAL_Driver/*.*")

    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32H743ZITX_FLASH.ld)
    add_link_options(-T ${LINKER_SCRIPT})

    add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

    set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
    set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

    add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
            COMMENT "Building ${HEX_FILE}
            Building ${BIN_FILE}")
endif ()
