int printf(const char *format, ...);
void *memcpy(void *dest, void const *src, unsigned long n);
unsigned long strlen(const char *s);
int atoi(const char *nptr);

int gg = 142;

struct TestStruct {
    int field1;
    int field2;
};

void store555(int *addr) {
    *addr = 554;
    (*addr)++;
    return;
}

int add(int a, int b) {
    if (a == b) {
        b = a;
    }
    return a + b;
}

int mysqr(int x) {
    return x * x;
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
    memcpy((void*)fixFmt, (void*)fmt, strlen(fmt) + 1);
    char *replace = fixFmt + 2;
    *replace = 's';
    fixFmt[1UL] = 'e';
    printf(fixFmt, y);
//    printf("%p\n", fixFmt);

    struct TestStruct sss;
    sss.field1 = 111;
    sss.field2 = 222;
    printf("%d %d\n", sss.field1, sss.field2);

    printf("%f\n", 2.3 + 0.7f);
    printf("%f\n", 2.3f + 0.7f);

    int a = 123;
    int b = -a;
    printf("%d %d\n", a, b);
    char c = 'b';
    c--;
    printf("%c %c\n", c, c);

    int cnt = 10;
    while (cnt) {
        cnt--;
    }

    int arr[2 + 2];

    int i = 0;
    while (i < 4) {
        arr[i] = i * i;
        i++;
    }
    for (int j = 0; j < 4; j++)
        printf("%d ", arr[j]);
    printf("\n");

    i = 3;
    while (1) {
        i--;
        printf("%d ", i);
        if (i == 0)
            break;
    }
    printf("\n");

    i = 0;
    do {
        printf("%d ", i);
        if (i == 0)
            i = 2;
        else
            i--;
    } while (i != 0);
    printf("\n");

    int k;
    for (k = 0;; k++) {
        printf("%d ", k);
        if (k == 4)
            break;
    }
    printf("\n");

    int (*f)(int) = mysqr;
    printf("5^2 = %d\n", f(5));

    int arrw4[4];
    arrw4[0] = 3;
    arrw4[1] = 0;
    arrw4[2] = 0;
    arrw4[3] = 0;
    *(2 + arrw4) = 9;
    *(arrw4 + 1) = 6;
    *((arrw4 + 4) - 1) = 12;
    printf("%d %d %d %d\n", arrw4[0], arrw4[1], arrw4[2], arrw4[3]);

    while (1) {
        return x + 0; // * 0
    }

    return 199;
}
