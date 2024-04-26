#include <stdio.h>

int add(int x, int y) {
    if (x > y) {
        x ++;
    } else {
        y ++;
    }

    return x + y;
}

int main() {

    int a = 1, b = 3;

    int c = add(a, b);

    printf("c = %d\n", c);

    return 0;
}