int printf(const char *fmt, ...);

void f(int c) {
    int i = 0, j = 0;
    while (i < c) {
        printf("%d: ", i);
        j = 0;
        nested:
        j++;
        printf(j < i ? "%d " : "%d\n", j + c * 2);
        if (j < i)
            goto nested;
        i++;
    }
}

int main() {
    f(5);
    return 0;
}
