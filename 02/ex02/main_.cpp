#include <iostream>
#include "Fixed.hpp"

// コンストラクタ/デストラクタのログが間に挟まるので、判定行は [ OK ] / [FAIL] で目印を付ける。

static int g_failed = 0;

static void banner(char const *name) {
    std::cout << std::endl << "--- " << name << " ---" << std::endl;
}

static void check(char const *label, bool ok) {
    if (!ok)
        ++g_failed;
    std::cout << (ok ? "[ OK ] " : "[FAIL] ") << label << std::endl;
}

static void subjectExample(void) {
    banner("subject: 0 / 0.00390625 / 0.00390625 / 0.00390625 / 0.0078125 / 10.1016 / 10.1016");

    Fixed a;
    Fixed const b(Fixed(5.05f) * Fixed(2));

    std::cout << a << std::endl;
    std::cout << ++a << std::endl;
    std::cout << a << std::endl;
    std::cout << a++ << std::endl;
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << Fixed::max(a, b) << std::endl;
}

static void comparisonTests(void) {
    banner("comparison");

    Fixed const one(1);
    Fixed const two(2);
    Fixed const alsoTwo(2);
    Fixed const epsilonAbove(1.00390625f);
    Fixed const belowEpsilon(1.001f);
    Fixed const minusFive(-5);
    Fixed const minusOne(-1);

    check("1 < 2", one < two);
    check("2 > 1", two > one);
    check("1 <= 2 and 2 <= 2", one <= two && two <= alsoTwo);
    check("2 >= 1 and 2 >= 2", two >= one && two >= alsoTwo);
    check("2 == 2", two == alsoTwo);
    check("1 != 2", one != two);
    check("not (2 < 2) and not (2 > 2)", !(two < alsoTwo) && !(two > alsoTwo));
    check("1 < 1 + epsilon", one < epsilonAbove);
    check("1 == 1.001 (epsilon より小さい差は区別できない)", one == belowEpsilon);
    check("-5 < -1", minusFive < minusOne);
}

static void arithmeticTests(void) {
    banner("arithmetic");

    Fixed const a(0.1f);
    Fixed const b(0.2f);
    Fixed const c(0.3f);

    // float なら 0.1 + 0.2 != 0.3 だが、固定小数点の加算は生値の整数加算なので厳密に一致する。
    check("0.1 + 0.2 == 0.3 (加算は誤差ゼロ)", (a + b) == c);

    Fixed const x(7.5f);
    Fixed const y(2.25f);
    check("(x + y) - y == x (逆演算が成り立つ)", ((x + y) - y) == x);

    check("5.05 * 2 の生値が 2586", (Fixed(5.05f) * Fixed(2)).getRawBits() == 2586);
    check("x * 1 == x", (x * Fixed(1)) == x);
    check("x * 0 == 0", (x * Fixed(0)) == Fixed(0));
    check("10 / 4 == 2.5", (Fixed(10) / Fixed(4)) == Fixed(2.5f));
    check("1 / 3 の生値が 85 (= 0.332031)", (Fixed(1) / Fixed(3)).getRawBits() == 85);

    check("-2 * 3 == -6", (Fixed(-2) * Fixed(3)) == Fixed(-6));
    check("-2 * -3 == 6", (Fixed(-2) * Fixed(-3)) == Fixed(6));
    check("-10 / 4 == -2.5", (Fixed(-10) / Fixed(4)) == Fixed(-2.5f));
    check("3 - 5 == -2", (Fixed(3) - Fixed(5)) == Fixed(-2));

    // int 型のまま掛けると 256000 * 256000 が桁あふれする。long 中間の検証。
    check("1000 * 1000 == 1000000 (中間の桁あふれなし)",
          (Fixed(1000) * Fixed(1000)).toInt() == 1000000);
}

static void incrementTests(void) {
    banner("increment / decrement");

    Fixed a;
    check("前置 ++ の戻り値が加算後", (++a).getRawBits() == 1);
    check("前置 ++ の後、本体も加算済み", a.getRawBits() == 1);

    Fixed b;
    check("後置 ++ の戻り値は加算前", (b++).getRawBits() == 0);
    check("後置 ++ の後、本体は加算済み", b.getRawBits() == 1);

    Fixed c;
    ++(++c);
    check("++(++c) で 2 進む (参照を返している証拠)", c.getRawBits() == 2);

    Fixed d;
    --d;
    check("-- は負の向きへ進む", d.getRawBits() == -1);

    Fixed e;
    check("後置 -- の戻り値は減算前", (e--).getRawBits() == 0);
    check("後置 -- の後、本体は減算済み", e.getRawBits() == -1);

    Fixed f;
    ++f;
    check("epsilon は 0.00390625", f.toFloat() == 0.00390625f);
}

static void minMaxTests(void) {
    banner("min / max");

    Fixed x(1);
    Fixed y(2);
    Fixed const cx(1);
    Fixed const cy(2);

    check("min(1, 2) == 1", Fixed::min(x, y) == x);
    check("max(1, 2) == 2", Fixed::max(x, y) == y);
    check("const 版 min", Fixed::min(cx, cy) == cx);
    check("const 版 max", Fixed::max(cx, cy) == cy);

    // 非 const と const の混在。const 版が無いとここでコンパイルエラーになる。
    check("max(非 const, const) が呼べる", Fixed::max(x, cy) == cy);

    // 参照を返しているので、結果を書き換えると元が変わる。
    Fixed::max(x, y).setRawBits(0);
    check("max の戻り値は参照 (y が書き換わる)", y.getRawBits() == 0);

    Fixed p(3);
    Fixed q(3);
    check("同値のときは a を返す (min)", &Fixed::min(p, q) == &p);
    check("同値のときは a を返す (max)", &Fixed::max(p, q) == &p);
}

int main(void) {
    subjectExample();
    comparisonTests();
    arithmeticTests();
    incrementTests();
    minMaxTests();

    std::cout << std::endl << "=== failed: " << g_failed << " ===" << std::endl;
    return g_failed == 0 ? 0 : 1;
}
