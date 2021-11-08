extern int printf(const char *fmt, ...);
extern int atoi(const char *str);

int main(int argc, char **argv) {
    printf("Hello world %d\n", 3);
    if (argc != 2)
        printf("Usage: tst1 <num>\n");
    volatile int x = 0;
    x += 2 * (atoi(argv[1]) + 4);
    printf("x = %d\n", x);
    for (int i = 0; i < 10; ) {
        printf("i=%d\n", i);
        i++;
    }
    return 0;
}
