#!/usr/bin/env bash
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/harl"

cd "$ROOT"

if [[ ! -x "$BIN" ]]; then
    echo "run 'make' first" >&2
    exit 1
fi

pass=0
fail=0

assert_contains() {
    local name="$1"
    local haystack="$2"
    local needle="$3"
    if [[ "$haystack" == *"$needle"* ]]; then
        echo "PASS: $name"
        pass=$((pass + 1))
    else
        echo "FAIL: $name (missing: $needle)" >&2
        fail=$((fail + 1))
    fi
}

out="$("$BIN")"

assert_contains "DEBUG header" "$out" "[ DEBUG ]"
assert_contains "INFO header" "$out" "[ INFO ]"
assert_contains "WARNING header" "$out" "[ WARNING ]"
assert_contains "ERROR header" "$out" "[ ERROR ]"
assert_contains "manager line" "$out" "manager now."

echo "---"
echo "passed: $pass failed: $fail"
[[ "$fail" -eq 0 ]]
