int add(int a, int b) {
    if (a == b)
        return a + a;
    return a + b;
}

int main() {
    int x = 2;
    int y = 6;
    while (x < y) {
        x++;
        y--;
        if (x == y)
            continue;
        else
            break;
    }
    return 0;
}
