#!/bin/bash

awk '{ if ($2) { print "{\"name\":\"" $1 "\",\"lines\":[" $2 "]}" } else { print "{\"name\":\"" $1 "\"}" } }' | paste -s -d, -
