#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
#include <stdio.h>
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x+y; }
int sub(int x, int y) { return x-y; }
int add6(int a, int b, int c, int d, int e, int f) {
  return a+b+c+d+e+f;
}
EOF

./9cc test.c > tmp.s
cc -static -o tmp tmp.s tmp2.o
./tmp

if [ "$?" = 0 ]; then
  echo OK
else
  exit 1
fi
