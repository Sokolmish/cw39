#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

void FUT(int a1, int a2, int a3);

int main(int argc, char **argv) {
    srand(0xdeadfeed);

    struct timeval tval_before, tval_after, tval_result;

    gettimeofday(&tval_before, NULL);
    FUT(0, 0, 0);
    gettimeofday(&tval_after, NULL);

    timersub(&tval_after, &tval_before, &tval_result);
    fprintf(stderr, "Time elapsed: %ld.%06lds\n", (long)tval_result.tv_sec, (long)tval_result.tv_usec);

    return 0;
}
