int printf(const char *fmt, ...);

void f(int c) {
    int i = 0;
loop:
    if (i < c * 2)
        printf("%d ", i);
    else
        printf("%d\n", i);
    if (i++ < c * 2)
        goto loop;
}

int main() {
    f(3);
    return 0;
}
