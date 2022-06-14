int printf(const char *format, ...);
int atoi(const char *nptr);
int rand(void);

typedef unsigned int u32;

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

int FUT(int a1, int a2, int a3) {
    u32 arg;
    u32 space;

    for (int i = 0; i < 5000000; i++) { // 1000000000
        int x1 = 0, x3 = 0;

        arg = rand() << (rand() % 30);
        if (arg == 0)
            arg = 1;

        x1 = __builtin_ctz(arg);
        x3 = count_zeroes2(arg);

        stop_dce(&space, x1);
        stop_dce(&space, x3);
    }
    return 0;
}
