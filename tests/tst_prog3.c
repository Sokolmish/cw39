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

int main(int argc, char **argv) {
    int arr[6] = { 10, 11, 12, 13, 14, 15 };
    int sz = 6;

    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            printf("%d ", arr[i]);
            if (j == i)
                break;
        }

        printf("\n");
    }

    return 0;
}
