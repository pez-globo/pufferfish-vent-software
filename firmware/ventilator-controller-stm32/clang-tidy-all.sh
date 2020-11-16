#!/bin/bash

CMAKE_BUILD_DIR=`echo "cmake-build-$1" | awk '{print tolower($0)}'`
if [ ! -f "$CMAKE_BUILD_DIR/compile_commands.json" ]; then
  echo 'Please use `./cmake.sh` to create a build folder for the $1 build type!'
  exit 1
fi

COMPILE_COMMANDS_DATABASE="-p $CMAKE_BUILD_DIR"
STANDARD_ARGS="$COMPILE_COMMANDS_DATABASE ${@:2}"

# Set file filters
IGNORE_FILES="stm32h7xx_hal_conf.h \
  Core/Inc/nanopb/pb.h \
  Core/Inc/nanopb/pb_common.h \
  Core/Inc/nanopb/pb_decode.h \
  Core/Inc/nanopb/pb_encode.h \
  Core/Inc/Pufferfish/Application/mcu_pb.h \
  Core/Src/Pufferfish/Application/mcu_pb.c"
STM32_USER_FILES="Core/Inc/main.h \
  Core/Src/main.cpp \
  Core/Inc/stm32h7xx_it.h \
  Core/Src/stm32h7xx_hal_msp.c \
  Core/Src/stm32h7xx_it.cpp"
INCLUDE_EXTENSIONS=".h .cpp .tpp"

# Set line filters based on file filters
EXCLUDE_FILTERS=`echo $IGNORE_FILES | tr ' ' '\n' \
  | awk '{print $1 " [1,1]"}' \
  | ./clang-tidy-line-filters.sh`
INCLUDE_FILTERS=`echo $INCLUDE_EXTENSIONS | tr ' ' '\n' \
  | ./clang-tidy-line-filters.sh`
STM32_USER_FILTERS=$(
  for FILE in $STM32_USER_FILES; do
    LINES=`./grep-stm32cubemx-user.sh "$FILE" \
      | awk 'BEGIN {FS=":"}; {print "[" $1 "," $2 "]" }' \
      | paste -s -d, -`
    echo "$FILE $LINES"
  done
)
STM32_USER_FILTERS=`echo "$STM32_USER_FILTERS" | ./clang-tidy-line-filters.sh`
LINE_FILTERS="[$EXCLUDE_FILTERS,$STM32_USER_FILTERS,$INCLUDE_FILTERS]"

# Check files
SOURCE_FILES=`find Core/Src/Pufferfish -iname *.cpp | xargs`
if [ "$1" == "TestCatch2" ]; then
  TEST_FILES=`find Core/Test -iname *.cpp | xargs`
  FILES="$TEST_FILES"
  CHECK_OVERRIDES="-readability-magic-numbers"
elif [ "$1" == "Clang" ]; then
  FILES="$SOURCE_FILES $STM32_USER_FILES"
  CHECK_OVERRIDES=""
else
  echo "$1 is not a valid target for clang-tidy checks"
  exit 1
fi
echo clang-tidy $FILE --line-filter="$LINE_FILTERS" $CHECK_OVERRIDES $STANDARD_ARGS
clang-tidy $FILES --line-filter="$LINE_FILTERS" --checks=$CHECK_OVERRIDES $STANDARD_ARGS
