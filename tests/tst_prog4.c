#define HELLO
#define WORLD
#undef HELLO


//#line 666 "xxx"

#include "incl_file.h"

#define BEGIN_STR "Begin\n"
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
    printf(BEGIN_STR);

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

    printf("Con\\n" "Cat\n");
    printf("Functi\x6Fn: %s\n", __func__);
    printf(/*hehe*/"End %s %d\n", __FILE__, __LINE__);
    printf("Date: %s\n", __DATE__);
    return 0;
}
