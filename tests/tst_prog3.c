int printf(const char *format, ...);
char *memcpy(char *dest, char const *src, unsigned long n); // Void pointers are not allowed
unsigned long strlen(const char *s);
int atoi(const char *nptr);

int main() {
    int x = 2;
    int y = x + 12;
    int z = (y + atoi("32")) * 16;
    return z;
}
