#!/usr/bin/env bash

make clean
make
if [ $? == 0 ]; then
  echo " r 'test/docs4/' resources/categories/ 3
  bt" | gdb -q bin/runner
fi
