// #include <stdio.h>
// #include "libsysy/libsysy.h"

const int A = 1;
const int B[2] = {2, 3}, C[1][1] = {{1}};

int a;
int b[2], c[1][1];

int d = 1;
int e[2] = {1}, f[1][1] = {{1}};

void void_func() {}

void void_return() {
    return;
}

int f1(int parameter) {
    int b;
    int array[2];
    b = getint();
    b + 1;
    a = b + 1;

    {}

    if (1 == 1)
        ;
    else
        ;

    if (2 == 2)
        ;

    for (array[0] = 1; !1 > 1; array[1] = 1)
        break;

    for (; 1 > 1; array[1] = 1)
        break;
    for (array[0] = 1;; array[1] = 1)
        break;
    for (array[0] = 1; 1 > 1;)
        break;
    for (;;)
        break;
    return 0;
}

int f2(int p1, int p2[], int p3[][5]) {
    return 0;
}

int f3(int p[2]) {
    return p[0];
}

int main() {
    /* code */
    int a;
    int b[1];
    int c[1][5];
    a = 1;
    b[0] = 1;
    c[0][0] = 1;

    1 * 2;

    a * (2 + 3) / 4 + 1 * f1(A);
    -+c[0][0] * 2 / 3 % 5;

    int x = 1 == 1 * 7 < 5;

    void_func();
    f1(2);
    f2(+-!(a == 0), b, c);

    // 数组传参
    int arr1[2] = {3, 4};
    f3(arr1);

    // 部分传参
    int arr2[2][2] = {{1, 2}, {3, 4}};
    f3(arr2[1]);

    // 条件表达式
    if (1)
        ;

    if (1 + 1 < 2 || a > 2 || a <= 2 && 1 >= 2)
        ;

    if (5 == 6 || 8 != 9 && 1 == 2 > 3)
        ;

    const int PI = 3 + 1 - A;

    printf("21371421\n");
    printf("Hello World!\n");
    printf("Hello World!%d\n", 5);

    return 0;
}
