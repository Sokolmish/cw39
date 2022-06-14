#include <stdio.h>
#include <sys/time.h>

void FUT(int a1, int a2, int a3);

int main(int argc, char **argv) {
    struct timeval tval_before, tval_after, tval_result;

    gettimeofday(&tval_before, NULL);
    FUT(0, 0, 0);
    gettimeofday(&tval_after, NULL);

    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06lds\n", (long)tval_result.tv_sec, (long)tval_result.tv_usec);

    return 0;
}
