#!/bin/bash
set -eu
cd /w
c++ -std=c++98 -O0 -S -o /tmp/r0.s _ret.cpp
c++filt < /tmp/r0.s > /tmp/r0f.s
grep -n "User::add\|Trivial::add" /tmp/r0f.s
echo "----- User::add body -----"
awk '
  /^User::add/ { capture=1; print; n=0; next }
  /^[A-Za-z_].*:$/ && capture { exit }
  capture && n<40 && $0 !~ /\.cfi/ { print; n++ }
' /tmp/r0f.s
echo "----- Trivial::add body -----"
awk '
  /^Trivial::add/ { capture=1; print; n=0; next }
  /^[A-Za-z_].*:$/ && capture { exit }
  capture && n<25 && $0 !~ /\.cfi/ { print; n++ }
' /tmp/r0f.s
