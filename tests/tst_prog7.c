int printf(const char *format, ...);
int atoi(const char *nptr);

typedef unsigned int u32;

int main() {
    u32 x = atoi("224");
    printf("CTZ: %u\n", __builtin_ctz(x));
    printf("CLZ: %u\n", __builtin_clz(x));
    printf("POPCNT: %u\n", __builtin_popcount(x));
    printf("BITREV32: %u\n", __builtin_bitreverse32(x));
    return 0;
}
