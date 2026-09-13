#!/usr/bin/env bash
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/replace_"
FIX="$ROOT/tests_/fixtures_"
TMP="$ROOT/tests_/tmp"

cd "$ROOT"

if [[ ! -x "$BIN" ]]; then
    echo "run: make -f Makefile_" >&2
    exit 1
fi

pass=0
fail=0

assert_eq() {
    local name="$1"
    local got="$2"
    local want="$3"
    if [[ "$got" == "$want" ]]; then
        echo "PASS: $name"
        pass=$((pass + 1))
    else
        echo "FAIL: $name" >&2
        echo "  want: [$want]" >&2
        echo "  got:  [$got]" >&2
        fail=$((fail + 1))
    fi
}

assert_exit() {
    local name="$1"
    local want="$2"
    shift 2
    set +e
    "$@" >/dev/null 2>&1
    local got=$?
    set -e
    if [[ "$got" -eq "$want" ]]; then
        echo "PASS: $name"
        pass=$((pass + 1))
    else
        echo "FAIL: $name (exit $got, want $want)" >&2
        fail=$((fail + 1))
    fi
}

cp "$FIX/simple.in" "$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "world" "42"
assert_eq "integration simple.in" "$(cat "$TMP.replace")" "$(cat "$FIX/simple.expected")"

printf 'aaaa' >"$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "aa" "b"
assert_eq "integration aaaa" "$(cat "$TMP.replace")" "ba"

assert_exit "argc too few" 1 "$BIN" onlyone
assert_exit "missing input file" 1 "$BIN" "$ROOT/tests_/no_such_file_42" "a" "b"

printf 'x' >"$TMP"
assert_exit "empty s1" 1 "$BIN" "$TMP" "" "y"

echo "---"
echo "passed: $pass failed: $fail"
[[ "$fail" -eq 0 ]]
