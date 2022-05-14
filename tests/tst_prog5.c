int printf(const char *format, ...);

//extern int ex;
int ten;
static int st;
int def = 10;

int f(void) {
    return ten + def + st;
}

static int rec(int x) {
    if (x < 1)
        return 0;
    printf("%d\n", x);
    return rec(x - 1);
}

int main() {
    rec(5);
    f();
    return 0;
}
