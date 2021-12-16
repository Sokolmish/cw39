//extern int printf(const char *fmt, ...);
//extern int atoi(const char *str);

int add(int a, int b) {
    return a + b;
}

int main(int argc, char **argv) {
//    struct Alpha { int f, g; } sss;
//    sss.g = sss.f + 12;

    int arr[10];
    arr[2] = arr[3];

    char *aa = *argv;
    *aa = (char)111;

    int x = 10;
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
