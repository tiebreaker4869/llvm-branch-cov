#include <cstdio>
#include <gtest/gtest.h>

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

TEST(TestCase, Test_1) {
    int a = add(1, 2);

    int b = add(6, 5);

    int c = add(3, 4);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
    return 0;
}