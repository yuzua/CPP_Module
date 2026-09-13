#!/usr/bin/env bash
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/replace"
FIX="$ROOT/tests/fixtures"
TMP="$ROOT/tests/tmp"
OUT="$ROOT/tests/out.txt"

cd "$ROOT"

if [[ ! -x "$BIN" ]]; then
    echo "run 'make' first" >&2
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

# T-06 integration: simple.in
cp "$FIX/simple.in" "$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "world" "42"
got="$(cat "$TMP.replace")"
want="$(cat "$FIX/simple.expected")"
assert_eq "integration simple.in" "$got" "$want"

# T-02 non-overlapping: aaaa / aa -> b  → bb（0-1 と 2-3 の2回。重なり 1-2 は見ない）
printf 'aaaa' >"$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "aa" "b"
assert_eq "integration aaaa (non-overlapping)" "$(cat "$TMP.replace")" "bb"

# T-apple: s2 に s1 が含まれても、置換後の apple_pie 内の apple を再置換しない
printf 'I like apple.' >"$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "apple" "apple_pie"
assert_eq "apple to apple_pie once" "$(cat "$TMP.replace")" "I like apple_pie."

# T-empty: 空ファイル → 空の .replace
: >"$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "x" "y"
assert_eq "empty input file" "$(cat "$TMP.replace")" ""

# パターン1
printf 'abc' >"$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "zzz" "Q"
assert_eq "no match copies input" "$(cat "$TMP.replace")" "abc"

# パターン2
printf 'hello world' >"$TMP"
rm -f "$TMP.replace"
"$BIN" "$TMP" "world" ""
assert_eq "s2 empty delete" "$(cat "$TMP.replace")" "hello "

#   パターン3
printf 'data' >"$TMP"
rm -rf "$TMP.replace"
mkdir "$TMP.replace"
assert_exit "output path not a writable file" 1 "$BIN" "$TMP" "a" "b"
rm -rf "$TMP.replace"

# T-05 wrong argc
assert_exit "argc too few" 1 "$BIN" onlyone

# T-07 missing file
assert_exit "missing input file" 1 "$BIN" "$ROOT/tests/no_such_file_42" "a" "b"

# empty s1
printf 'x' >"$TMP"
assert_exit "empty s1" 1 "$BIN" "$TMP" "" "y"

echo "---"
echo "passed: $pass failed: $fail"
[[ "$fail" -eq 0 ]]
