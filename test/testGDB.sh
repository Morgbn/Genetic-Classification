#!/usr/bin/env bash

make clean
make
if [ $? == 0 ]; then
  echo " r 'test/docs4/' 3 cats.txt
  bt" | gdb -q bin/runner
fi
