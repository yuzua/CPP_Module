#include <cstdio>
#include <cmath>
#include <stdint.h>

static void bits(char const *label, uint32_t u) {
    std::printf("%s ", label);
    for (int i = 31; i >= 0; --i) {
        std::printf("%u", (u >> i) & 1u);
        if (i == 31 || i == 23 || i == 8)
            std::printf(" ");
    }
    std::printf("\n");
}

int main(void) {
    float f = 42.42f;
    uint32_t u;
    __builtin_memcpy(&u, &f, 4);
    bits("42.42f ", u);
    int raw = static_cast<int>(roundf(f * 256.f));
    bits("raw    ", static_cast<uint32_t>(raw));
    std::printf("raw=%d back=%.10f\n", raw, raw / 256.f);

    float g = 1234.4321f;
    int rawg = static_cast<int>(roundf(g * 256.f));
    std::printf("1234 raw=%d back=%.10f\n", rawg, rawg / 256.f);

    int neg = static_cast<int>(roundf(-1.5f * 256.f));
    bits("neg    ", static_cast<uint32_t>(neg));
    std::printf("neg raw=%d\n", neg);

    int a = 512, b = 1, c = 128;
    int left = static_cast<int>((static_cast<long>(a) * b / 256) * c / 256);
    int right = static_cast<int>(static_cast<long>(a) * (static_cast<long>(b) * c / 256) / 256);
    std::printf("assoc left=%d right=%d\n", left, right);
    return 0;
}
