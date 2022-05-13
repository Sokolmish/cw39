int printf(const char *format, ...);
void* memcpy(void *dest, const void *src, unsigned long n);
unsigned long strlen(const char *s);
int atoi(const char *nptr);

int g;

struct MStruct {
    int x, y, z;
} ms2 = { 11, 22, 33 + 7 };

struct ms {
    int x;
    long y;
    int z;
};

void longF(long arg) {
    printf("LONG: %ld\n", arg);
}

int main(int argc, char **argv) {
    int arr[6] = { 10, 11, 12U, 13, 14, 15 };
    int sz = 6;

    for (int i = 0; i < sz; i++) {
        switch (i) {
            case 1:
                printf("one  :  ");
                break;
            case 3:
                printf("three:");
            case 9:
                printf("f ");
                break;
            case 5:
                printf("five :  ");
                break;
            default:
                printf("even :  ");
                break;
        }
        for (int j = 0; j < sz; j++) {
            printf("%d ", arr[i]);
            if (j == i)
                break;
        }

        printf("\n");
    }

    int tst = 0;

    skip:
    if (tst)
        goto exit;

    while (1) {
        printf("%d\n", tst++);
        if (tst == 5)
            goto skip;
    }

    exit:

    printf("REM: %d\n", (unsigned)atoi("20") % 16U);

    longF(atoi("699"));

    return 0;
}
