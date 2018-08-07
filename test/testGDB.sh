#!/usr/bin/env bash

tput reset
make clean
make
if [ $? == 0 ]; then
  echo " r 'test/docs3/'
  bt" | gdb -q bin/runner
fi
