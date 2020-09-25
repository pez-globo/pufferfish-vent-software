#!/bin/bash

./grep-stm32cubemx-user.sh "$1" \
  | awk '{print "--lines=" $0}'  \
  | xargs clang-format -style=file "${@}"
