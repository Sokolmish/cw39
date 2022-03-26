#define HELLO
#define WORLD
#undef HELLO


//#line 666

#include "incl_file.h"


//#define DO_WARN
//#define DO_ERR

#ifdef DO_WARN
#   warning "This is warning"
#endif

#ifdef DO_ERR
#   error "This is error"
#endif

int printf(const char *format, ...);
void* memcpy(void *dest, const void *src, unsigned long n);
unsigned long strlen(const char *s); /*
        woo
 */int atoi(const char *nptr);

int main() {
    printf("Begin\n");

#ifdef PHRASE
#ifdef HELLO
    printf("Hello\n");
#else
    printf("Bye\n");
#endif
#ifdef WORLD
    printf("World\n");
#endif
#endif

    printf(/*hehe*/"End\n");
    return 0;
}
