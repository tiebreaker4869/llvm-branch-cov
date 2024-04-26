#include <stdio.h>

int add(int x, int y) {
    if (x > y) {
        x ++;
    } else {
        y ++;
    }

    switch(x) {
        case 1:
            x ++;
            break;
        case 5:
            y ++;
            break;
        default:
            x ++;
            y ++;
            break;
    }

    return x + y;
}

int main() {

    int a = 1, b = 3;

    int c = add(a, b);

    printf("c = %d\n", c);

    c = add(a + 2, b - 1);

    printf("c = %d\n", c);

    c = add(1, b);

    c = add(5, b);

    return 0;
}