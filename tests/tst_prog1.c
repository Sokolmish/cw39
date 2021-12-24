int gg = 142;

int printf(const char *format, ...);
char *memcpy(char *dest, char const *src, unsigned long n); // Void pointers are not allowed
unsigned long strlen(const char *s);

void store555(int *addr) {
    *addr = 555;
    return;
}

int add(int a, int b) {
    if (a == b) {
        b = a;
    }
    return a + b;
}

int main() {
    int x = gg;
    int y;
    store555(&y);
    x += 1;
    if (x == 7) {
        y += 3;
        gg = 25;
    }
    else {
        x = add(x, 5);
    }
    y = add(x, y);
    char *fmt = "Re?: %d\n";
    char fixFmt[16];
    memcpy(fixFmt, fmt, strlen(fmt) + 1UL);
    char *replace = (char*)((unsigned long)(char*)&fixFmt + 2UL);
    *replace = 's';
    printf(fixFmt, y);
    printf("%p %p\n", fixFmt, replace);
    return x + 0; // * 0
}
