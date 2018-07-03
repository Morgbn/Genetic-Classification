#!/usr/bin/env bash

make clean
make
if [ $? == 0 ]; then
  echo " r
  bt" | gdb -q bin/runner
fi
