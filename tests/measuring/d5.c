int printf(const char *format, ...);
int atoi(const char *nptr);
int rand(void);
void *malloc(unsigned long size);
void free(void *ptr);
float logf(float x);

typedef unsigned int u32;

void stop_dce(u32 *addr, u32 x) {
    volatile u32 *vaddr = addr;
    *vaddr = x;
}

u32 space;

// FUT_D5
int FUT(int a1, int a2, int a3) {
    int n = rand() % 10000;
    volatile int *arr = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
        arr[i] = rand();

    int max = -100000000;
    int i = 0;
    loop:
    if (max > arr[i] + arr[n % 61] * arr[i + 10])
        max = arr[i] + arr[n % 61] * arr[i + 10];
    i++;
    if (i < n - 10)
        goto loop;


    stop_dce(&space, max);

    free((void*)arr);
    return 0;
}
