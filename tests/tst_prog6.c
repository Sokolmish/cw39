int printf(const char *format, ...);
int atoi(const char *nptr);

typedef unsigned long uint64_t;

void f(void) {
    printf("XXX\n");
}

int main() {
    uint64_t bound = atoi("5");
    for (int i = 0; i < bound * 2; i++) {
        printf("%d\n", i);
    }
    f();
    return 0;
}
