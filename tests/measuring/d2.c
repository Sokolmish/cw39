int printf(const char *format, ...);
int atoi(const char *nptr);
int rand(void);
void *malloc(unsigned long size);
void free(void *ptr);

typedef unsigned int u32;

void stop_dce(u32 *addr, u32 x) {
    volatile u32 *vaddr = addr;
    *vaddr = x;
}

int *array;

int compare(unsigned long i, unsigned long j) {
    if (i <= j) {
        printf("COMPARE %ld %ld\n", i, j);
    } else {
        printf("COMPARE %ld %ld\n", j, i);
    }

    if (array[i] == array[j]) return 0;
    return array[i] < array[j] ? -1 : 1;
}

void swap(unsigned long i, unsigned long j) {
    if (i <= j) {
        printf("SWAP %ld %ld\n", i, j);
    } else {
        printf("SWAP %ld %ld\n", j, i);
    }

    int t = array[i];
    array[i] = array[j];
    array[j] = t;
}

void bubblesort(unsigned long nel,
                int (*compare)(unsigned long i, unsigned long j),
                void (*swap)(unsigned long i, unsigned long j)) {
    int left = 0, right = nel - 1;
    int lr = 1;
    while (right > left) {
        if (lr) {
            int bound = right;
            right = left;
            for (int i = left; i < bound; ++i)
                if (compare(i, i + 1) > 0) {
                    swap(i, i + 1);
                    right = i;
                }
        }
        else {
            int bound = left;
            left = right;
            for (int i = right; i > bound; --i)
                if (compare(i - 1, i) > 0) {
                    swap(i - 1, i);
                    left = i;
                }
        }
        lr = !lr;
    }
}

int bubble(int n) {
    int i;

    array = (int*)malloc(n * sizeof(int));
    for (i = 0; i < n; i++)
        *(array + i) = rand();

    bubblesort(n, compare, swap);
    for (i = 0; i < n; i++)
        printf("%d", array[i]);
    printf("\n");

    free((void*)array);
    return 0;
}

// FUT_D2
int FUT(int a1, int a2, int a3) {
    bubble(100);
    return 0;
}
