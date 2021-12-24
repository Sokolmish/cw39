int gg = 142;

int* xxx() {
    return &gg;
}

int add(int a, int b) {
    if (a == b) {
        b = a;
    }
    return a + b;
}

int main() {
    int x = gg;
    int y = 6;
    x += 1;
    if (x == 7) {
        y = 3;
        gg = 25;
    }
    else {
        x = add(x, 5);
    }
    y = add(x, y);
    return x + 0; // * 0
}
