int printf(const char *format, ...);
int atoi(const char *nptr);
int rand(void);

typedef unsigned int u32;

void stop_dce(u32 *addr, u32 x) {
    volatile u32 *vaddr = addr;
    *vaddr = x;
}

// FUT_D1
int FUT(int a1, int a2, int a3) {
    int n = 10;
    int x0;
    x0 = rand() % 8;
    int res1 = 0;
    int res2 = 0;
    int cur;
    for (int i = n; i > 0; --i) {
        cur = rand() % 8;
        res1 = (res1 + cur) * x0;
        if (i == 1) {
            res2 += cur;
            break;
        }
        res2 = (res2 + cur * i) * x0;
    }
    cur = rand() % 8;
    printf("%d %d\n", res1 + cur, res2);
    return 0;
}
