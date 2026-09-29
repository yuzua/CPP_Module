#!/bin/bash
# 4 つの ex をまとめてビルド・実行する開発用スクリプト（提出物ではない）
set -u
cd "$(dirname "$0")"

for d in ex00 ex01 ex02 ex03; do
    echo "===================== $d ====================="
    (cd "$d" && make re) || exit 1
done

echo
echo "########## ex00 ##########"
(cd ex00 && ./fixed)
echo
echo "########## ex01 ##########"
(cd ex01 && ./fixed)
echo
echo "########## ex02 ##########"
(cd ex02 && ./fixed)
echo "ex02 exit: $?"
echo
echo "########## ex03 ##########"
(cd ex03 && ./bsp)
echo "ex03 exit: $?"
