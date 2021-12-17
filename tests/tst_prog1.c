//extern int printf(const char *fmt, ...);
//extern int atoi(const char *str);

int add(int a, int b) {
    return a + b;
}

int main(int argc, char **argv) {
    int *var;
    var = &argc;

    char *aa = *argv;
    *aa = (char)111;

    int x = *var;
    while (x) {
        x = x - 1;
        if (x) {
            break;
        }
        else
            continue;
    }
    return x;
}
