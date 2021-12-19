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
        x = 5;
    }
    return add(x, y);
}
