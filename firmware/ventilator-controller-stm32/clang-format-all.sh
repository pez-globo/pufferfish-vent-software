#!/bin/bash

# Clean up header files
find Core/Inc/Pufferfish -iname *.h -o -iname *.tpp `# Recursively find all matching files` \
  | xargs clang-format -style=file "$@" `# Run clang-format`

# Clean up source files
find Core/Src/Pufferfish -iname *.cpp `# Recursively find all matching files` \
  | xargs clang-format -style=file "$@" `# Run clang-format`

# # Clean up test files
find Core/Test -iname *.h -o -iname *.tpp -o -iname *.cpp `# Recursively find all matching files` \
  | xargs clang-format -style=file "$@" `# Run clang-format`

# Clean up STM32CubeMX auto-generated files
MX_GENERATED="Core/Inc/main.h \
  Core/Src/main.cpp \
  Core/Inc/stm32h7xx_it.h \
  Core/Src/stm32h7xx_hal_msp.c \
  Core/Src/stm32h7xx_it.cpp"
for FILE in $MX_GENERATED; do
  ./clang-format-stm32cubemx-user.sh "$FILE" "$@"
done
