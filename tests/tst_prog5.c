int printf(const char *format, ...);

static int rec(int x) {
    if (x < 1)
        return 0;
    printf("%d\n", x);
    return rec(x - 1);
}

int main() {
    rec(5);
    return 0;
}
