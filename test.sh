#!/bin/sh

T2_CONFIG=macosx-clang
T2_VARIANT=debug

run_test() {
  T2_OUTPUT=`tundra2 $T2_CONFIG-$T2_VARIANT-default $1`
  if [ $? -ne 0 ]; then
    EXIT_CODE=$?
    echo $T2_OUTPUT
    exit $EXIT_CODE
  fi
  TEST_OUTPUT=`t2-output/$T2_CONFIG-$T2_VARIANT-default/$1`
  printf "%-20s " $1
  if [ $? -ne 0 ]; then
    EXIT_CODE=$?
    printf "[ \e[31m%s\e[0m ]\n" "FAIL"
    echo $TEST_OUTPUT
    exit $EXIT_CODE
  fi
  printf "[  \e[32m%s\e[0m  ]\n" "OK"
}

for fn in src/*_test.c; do
  TEST_NAME=`basename -s .c $fn`
  run_test $TEST_NAME
done
