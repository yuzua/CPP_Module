#include <iostream>
#include "Point.hpp"

static int g_failed = 0;

static void banner(char const *name) {
    std::cout << std::endl << "--- " << name << " ---" << std::endl;
}

static void check(char const *label, bool ok) {
    if (!ok)
        ++g_failed;
    std::cout << (ok ? "[ OK ] " : "[FAIL] ") << label << std::endl;
}

static void runCase(char const *label, Point const &a, Point const &b, Point const &c,
                    Point const &p, bool expected) {
    bool const got = bsp(a, b, c, p);

    if (got != expected)
        ++g_failed;
    std::cout << (got == expected ? "[ OK ] " : "[FAIL] ")
              << "(" << p.getX() << ", " << p.getY() << ") -> "
              << (got ? "true " : "false")
              << "  expected " << (expected ? "true " : "false")
              << "  | " << label << std::endl;
}

// 基準の三角形: a(0,0) b(10,0) c(0,10) の直角三角形
static void rightTriangleTests(void) {
    banner("triangle (0,0) (10,0) (0,10)");

    Point const a(0.0f, 0.0f);
    Point const b(10.0f, 0.0f);
    Point const c(0.0f, 10.0f);

    runCase("内側", a, b, c, Point(1.0f, 1.0f), true);
    runCase("内側（重心付近）", a, b, c, Point(3.33f, 3.33f), true);
    runCase("外側（遠い）", a, b, c, Point(20.0f, 20.0f), false);
    runCase("外側（左に僅か）", a, b, c, Point(-0.1f, 5.0f), false);

    runCase("頂点 a の上", a, b, c, Point(0.0f, 0.0f), false);
    runCase("頂点 b の上", a, b, c, Point(10.0f, 0.0f), false);
    runCase("頂点 c の上", a, b, c, Point(0.0f, 10.0f), false);

    runCase("辺 ac（縦）の上", a, b, c, Point(0.0f, 5.0f), false);
    runCase("辺 ab（横）の上", a, b, c, Point(5.0f, 0.0f), false);
    runCase("辺 bc（斜辺）の上", a, b, c, Point(5.0f, 5.0f), false);

    runCase("斜辺のすぐ内側", a, b, c, Point(4.99f, 5.0f), true);
    runCase("斜辺のすぐ外側", a, b, c, Point(5.01f, 5.0f), false);
    runCase("斜辺から epsilon だけ内側", a, b, c, Point(4.99609375f, 5.0f), true);
}

static void orientationTests(void) {
    banner("頂点の並び順に依存しないこと");

    Point const a(0.0f, 0.0f);
    Point const b(10.0f, 0.0f);
    Point const c(0.0f, 10.0f);

    runCase("反時計回り a,b,c", a, b, c, Point(1.0f, 1.0f), true);
    runCase("時計回り a,c,b", a, c, b, Point(1.0f, 1.0f), true);
    runCase("反時計回り・辺上", a, b, c, Point(5.0f, 5.0f), false);
    runCase("時計回り・辺上", a, c, b, Point(5.0f, 5.0f), false);
}

static void negativeCoordinateTests(void) {
    banner("triangle (-5,-5) (5,-5) (0,5)");

    Point const a(-5.0f, -5.0f);
    Point const b(5.0f, -5.0f);
    Point const c(0.0f, 5.0f);

    runCase("原点は内側", a, b, c, Point(0.0f, 0.0f), true);
    runCase("下にはみ出す", a, b, c, Point(0.0f, -6.0f), false);
    runCase("底辺の上", a, b, c, Point(0.0f, -5.0f), false);
}

static void degenerateTests(void) {
    banner("退化した三角形");

    Point const a(0.0f, 0.0f);
    Point const b(1.0f, 1.0f);
    Point const c(2.0f, 2.0f);

    runCase("3 点が一直線（面積 0）", a, b, c, Point(1.0f, 1.0f), false);

    Point const z(0.0f, 0.0f);
    runCase("3 点が同一", z, z, z, Point(0.0f, 0.0f), false);

    Point const s0(0.0f, 0.0f);
    Point const s1(0.01171875f, 0.0f);        // epsilon 3 個分
    Point const s2(0.0f, 0.01171875f);
    runCase("極小の三角形の内側", s0, s1, s2, Point(0.00390625f, 0.00390625f), true);
}

static void pointClassTests(void) {
    banner("Point クラス");

    Point const origin;
    check("デフォルトコンストラクタは (0, 0)",
          origin.getX() == Fixed(0) && origin.getY() == Fixed(0));

    Point const p(1.0f, 2.0f);
    Point const copy(p);
    check("コピーコンストラクタが値を複製する",
          copy.getX() == p.getX() && copy.getY() == p.getY());

    // _x / _y が const なので、代入しても値は変わらない（意図した no-op）。
    Point q(3.0f, 4.0f);
    q = p;
    check("代入演算子は呼べるが値は変わらない（const メンバのため）",
          q.getX() == Fixed(3) && q.getY() == Fixed(4));
}

int main(void) {
    rightTriangleTests();
    orientationTests();
    negativeCoordinateTests();
    degenerateTests();
    pointClassTests();

    std::cout << std::endl << "=== failed: " << g_failed << " ===" << std::endl;
    return g_failed == 0 ? 0 : 1;
}
