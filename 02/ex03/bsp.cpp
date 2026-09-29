#include "Point.hpp"

// 2D の外積 (p - origin) x (q - origin)。
//
// 符号 > 0 : q は origin->p の左側
// 符号 < 0 : q は origin->p の右側
// 符号 = 0 : 3 点が同一直線上
//
// 必要なのは符号だけで大きさは使わないので、倍率 2^8 を戻す必要がない。
// つまり生値のまま掛けてよく、丸めが一度も起きない（完全に厳密）。
// 生値どうしの積は int に収まらないので long で受ける。
static long cross(Point const &origin, Point const &p, Point const &q) {
    long const px = (p.getX() - origin.getX()).getRawBits();
    long const py = (p.getY() - origin.getY()).getRawBits();
    long const qx = (q.getX() - origin.getX()).getRawBits();
    long const qy = (q.getY() - origin.getY()).getRawBits();

    return px * qy - py * qx;
}

bool bsp(Point const a, Point const b, Point const c, Point const point) {
    long const d1 = cross(a, b, point);
    long const d2 = cross(b, c, point);
    long const d3 = cross(c, a, point);

    // どれかが 0 = 辺（またはその延長）の上。頂点上もここに入る。
    if (d1 == 0 || d2 == 0 || d3 == 0)
        return false;

    // 3 辺すべてに対して同じ側にあれば内側。
    // 全部正 / 全部負の両方を見ているので、頂点の並び順（時計回り・反時計回り）に依存しない。
    return (d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0);
}
