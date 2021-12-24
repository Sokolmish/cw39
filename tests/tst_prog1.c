int gg = 142;

struct TestStruct {
    int field1;
    int field2;
};

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
    char *fmt = "R??: %d\n";
    char fixFmt[16];
    memcpy(fixFmt, fmt, strlen(fmt) + 1UL);
    char *replace = (char*)((unsigned long)(char*)&fixFmt + 2UL);
    *replace = 's';
    fixFmt[1UL] = 'e';
    printf(fixFmt, y);
    printf("%p\n", fixFmt);

    struct TestStruct sss;
    sss.field1 = 111;
    sss.field2 = 222;
    printf("%d %d\n", sss.field1, sss.field2);

    printf("%f\n", 2.3 + (double)0.7f);

    return x + 0; // * 0
}
