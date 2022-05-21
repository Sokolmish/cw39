int printf(const char *format, ...);
int atoi(const char *nptr);

typedef unsigned int u32;

//int count_zeroes1(u32 mask) {
//    int cnt = 0;
//    while ((mask & 1) == 0) {
//        mask >>= 1;
//        ++cnt;
//    }
//    return cnt;
//}

int count_zeroes2(u32 mask) {
    u32 cnt = 0;
    while (((mask >> cnt) & 1) == 0) {
        ++cnt;
    }
    return cnt;
}

int main() {
    u32 arg = atoi("800");
    int x1 = 0, x2 = 0, x3 = 0;
    x1 = __builtin_ctz(arg);
//    x2 = count_zeroes1(arg);
    x3 = count_zeroes2(arg);
    printf("%d %d %d\n", x1, x2, x3);
    return 0;
}

//int count_zeroes3(u32 mask) {
//    u32 cnt = 0;
//    int x = 0;
//    while (((mask >> cnt) & 1) == 0) {
//        ++cnt;
//        x++;
//    }
//    printf("%d\n", x);
//    return cnt;
//}
