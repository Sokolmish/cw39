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

void kadane(float *arr, int n, int *l, int *r) {
    *l = 0;
    *r = 0;
    float maxsum = arr[0], sum = 0.f;
    int start = 0;
    int i = 0;
    while (i < n) {
        sum += arr[i];
        if (sum > maxsum) {
            maxsum = sum;
            *l = start;
            *r = i;
        }
        ++i;
        if (sum < 0.f) {
            sum = 0.f;
            start = i;
        }
    }
}

// FUT_D3
int FUT(int a1, int a2, int a3) {
    int n = 10000;
    float *arr = (float*)malloc(n * sizeof(float));
    for (int i = 0; i < n; ++i) {
        int t1, t2;
//        scanf("%i/%i ", &t1, &t2);
        t1 = rand() % 16;
        t2 = rand() % 16;
        arr[i] = logf((float)t1 / t2);
    }
    int l, r;
    kadane(arr, n, &l, &r);
//    printf("%i %i", l, r);

    u32 space;
    stop_dce(&space, l);
    stop_dce(&space, r);
    stop_dce(&space, (int)(unsigned long)arr);

    free((void*)arr);
    return 0;
}
