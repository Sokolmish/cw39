int add(int a, int b) {
    if (a == b) {
        b = a;
    }
    return a + b;
}

int main() {
    int x = 2;
    int y = 6;
    if (x == 7) {
        y = 3;
    }
    else {
        x = add(x, 5);
    }
    y = add(x, y);
    return x + 0; // * 0
}
