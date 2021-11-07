int printf(const char * __restrict fmt, ...);

int main(int argc, char **argv) {
    printf("Hello world %d\n", 3);
    register int x = 0;
    return 0;
}
