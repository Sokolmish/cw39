int printf(const char *format, ...);
unsigned long strlen(const char *s);
int atoi(const char *nptr);

int tr(int x) {
    printf("[true %d]", x);
    return 1;
}

int fl(int x) {
    printf("[false %d]", x);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Wrong arguments count\n");
        return 1;
    }

    int x = atoi(argv[1]); // 1
    int y = atoi(argv[2]); // 2
    int z = atoi(argv[3]); // 3

    printf("%d %d %d\n", x, y, z);

    printf("%d\n", tr(1) && fl(2));
    printf("%d\n", fl(1) && fl(2));
    printf("%d\n", tr(1) || fl(2));
    printf("%d\n", fl(1) || tr(2));
    printf("%d\n", tr(1) && fl(2) || tr(3));
    printf("%d\n", fl(1) && (tr(2) || tr(3)));

    if (tr(1) && fl(2))
        printf("true\n");
    else
        printf("false\n");

    return 0;
}
