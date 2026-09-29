#!/bin/bash
set -eu
cd /w
c++ -std=c++98 -O2 -S -o /tmp/p.s _asm_probe.cpp
c++ -std=c++98 -O0 -S -o /tmp/p0.s _asm_probe.cpp
echo "===== O2 functions ====="
awk '
  /^[^ \t].*:$/ { name=$0; buf=""; n=0; capture=0
    if (name ~ /add_raw|mul_raw|to_int|scale|fadd|fmul|idiv_var|call_add|addERK|mulERK|divERK|toIntEv|scaledEi/) capture=1
    next
  }
  capture {
    if ($0 ~ /^\s*\./) next
    if (n < 28) { buf = buf $0 "\n"; n++ }
  }
  /^[^ \t].*:$/ { }
  END {}
' /tmp/p.s
# simpler: c++filt and sed ranges
c++filt < /tmp/p.s > /tmp/pf.s || cp /tmp/p.s /tmp/pf.s
echo "----- filtered O2 -----"
awk '
  /^[A-Za-z_].*\(.*\):$/ {
    name=$0
    capture = (name ~ /^(add_raw|mul_raw|to_int|scale|fadd|fmul|idiv_var|call_add)\(/ || name ~ /^F::(add|mul|div|toInt|scaled)/)
    if (capture) { print "#### " name; n=0 }
    else capture=0
    next
  }
  capture && n<18 { print; n++ }
' /tmp/pf.s
echo "----- O0 -----"
c++filt < /tmp/p0.s > /tmp/p0f.s
awk '
  /^[A-Za-z_].*\(.*\):$/ {
    name=$0
    capture = (name ~ /^call_add\(/ || name ~ /^F::add/)
    if (capture) { print "#### " name; n=0 }
    else capture=0
    next
  }
  capture && n<24 && $0 !~ /\.cfi/ { print; n++ }
' /tmp/p0f.s
echo "===== bits ====="
c++ -std=c++98 -O2 -o /tmp/bits _bits_probe.cpp
/tmp/bits
echo "===== sections ====="
echo 'static const int k = 8; int f(){return k;}' > /tmp/st.cpp
c++ -std=c++98 -c -o /tmp/st.o /tmp/st.cpp
objdump -t /tmp/st.o | awk '/k/'
objdump -s -j .rodata /tmp/st.o 2>/dev/null | head -20 || true
nm /tmp/st.o
