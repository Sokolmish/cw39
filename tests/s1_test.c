int printf(const char *format, ...);
int atoi(const char *nptr);

typedef unsigned int u32;

__attribute__((noinline))
void stop_dce(u32 *addr, u32 x) {
    volatile u32 *vaddr = addr;
    *vaddr = x;
}

static inline int count_zeroes2(u32 mask) {
    u32 cnt = 0;
    while (((mask >> cnt) & 1) == 0) {
        ++cnt;
    }
    return cnt;
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 0;
    u32 arg = atoi(argv[1]);
    u32 space;
    for (int i = 0; i < 5000000; i++) { // 1000000000
        int x1 = 0, x3 = 0;
        if (i % 2 == 0)
            arg += 4;
        else
            arg -= 4;
        x1 = __builtin_ctz(arg);
        x3 = count_zeroes2(arg);
//        printf("%d %d\n", x1, x3);
        stop_dce(&space, x1);
        stop_dce(&space, x3);
    }
    return 0;
}
