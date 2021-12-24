int gg = 142;

int printf(const char *format, ...);

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
    printf("%d\n", y);
    return x + 0; // * 0
}
