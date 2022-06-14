int printf(const char *format, ...);
int atoi(const char *nptr);
int rand(void);

typedef unsigned int u32;

__attribute__((noinline))
void stop_dce(u32 *addr, u32 x) {
    volatile u32 *vaddr = addr;
    *vaddr = x;
}

static inline int cycle(u32 len1, u32 len2) {
    int c = 0;
    for (int i = 0; i < len1; i += 3) {
        if (c < len2) {
            c += 6;
            printf("some\n");
        }
    }
    return c;
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 0;
    u32 arg;
    u32 space;

    arg = atoi(argv[1]);

    for (int i = 0; i < 5000000; i++) { // 1000000000
        int x1 = 0;

        arg = rand() << (rand() % 30);
        if (arg == 0)
            arg = 1;
//        x1 = cycle(arg);
        stop_dce(&space, x1);
    }
    return 0;
}

// clang-10 -O3 -S -emit-llvm ./test2.c
// opt-13 -dot-cfg test2.ll -disable-output -enable-new-pm=0
// dot -Tsvg -o g.svg .f.dot
