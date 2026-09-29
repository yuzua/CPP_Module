struct F {
    int v;
    int add(F const &rhs) const { return v + rhs.v; }
    int mul(F const &rhs) const {
        long p = static_cast<long>(v) * static_cast<long>(rhs.v);
        return static_cast<int>(p / 256);
    }
    int div(F const &rhs) const {
        long q = static_cast<long>(v) * 256;
        return static_cast<int>(q / rhs.v);
    }
    int toInt(void) const { return v >> 8; }
    int scaled(int x) const { return x * 256; }
};

int add_raw(int a, int b) { return a + b; }
int mul_raw(int a, int b) {
    long p = static_cast<long>(a) * static_cast<long>(b);
    return static_cast<int>(p / 256);
}
int to_int(int v) { return v >> 8; }
int scale(int x) { return x * 256; }
float fadd(float a, float b) { return a + b; }
float fmul(float a, float b) { return a * b; }
int idiv_var(int a, int b) { return a / b; }

static int call_add(F const &a, F const &b) { return a.add(b); }
