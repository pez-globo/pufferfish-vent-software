#!/bin/bash

grep -n '/* USER CODE ' "$1" `# Find all user section delimiters` \
  | cut -f1 -d: | paste -d ":" - - `# Get line numbers of start and end delimiters of each section` \
  | awk -F, 'BEGIN {FS=":"} ($1 + 1) != $2' `# Delete empty sections` \
  | awk 'BEGIN {FS=":"}; {print ($1 + 1) ":" ($2 - 1)}' `# Exclude section delimiters from sections` \
  | awk -F, 'BEGIN {FS=":"} $1 != $2' `# Delete empty sections` \

