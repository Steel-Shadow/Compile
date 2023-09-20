#include <stdio.h>
#include "lib/libsysy.h"

const int A = 1;

int a;
int b = 1;

void void_func() {}

void void_return() {
    return;
}

int f1(int parameter) {
    int A = 1;
    int B = 2, C = 3;
    int a;
    a = 1;

    1 * 2;

    a * (2 + 3 - 3) * 3 / 4;

    int x = 1 == 1 * 7 < 5;

    const int PI = 3 + 1 - A;

    int b;
    b + 1;
    a = b + 1;

    {};
    { ; }

    if (1 == 1)
        ;
    else
        ;

    if (2 == 2)
        ;

    for (a = 1; !1 > 1; b = 1)
        break;

    for (; 1 > 1; b = 1)
        break;
    for (a = 1;; b = 1)
        break;
    for (a = 1; 1 > 1;)
        break;
    for (;;)
        break;
    return 0;
}

int f2(int p1, int p2) {
    return p1 + p2;
}

int main() {
    printf("21371421\n");
    /* code */
    int a;
    a = 1;

    int b;
    b = getint();
    printf("%d\n", b);

    for (a = 1; a <= 10; a = a + 1)
        break;
    for (; a <= 10; a = a + 1)
        break;
    for (a = 1;; a = a + 1)
        break;
    for (a = 1; a <= 10;)
        break;
    for (a = 1;;)
        break;
    for (; a <= 10;)
        break;
    for (;; a = a + 1)
        break;
    for (;;) {
        for (;;) {
            for (;;)
                break;
            break;
        }
        break;
    }

    1 * 2;

    a * (2 + 3 - 3) * 3 / 4;

    f1(A);
    f2(a, -+-a);

    int x = 1 == 1 * 7 < 5;

    void_func();
    f1(2);
    f2(+-!(a == 0), a);

    const int PI = 3 + 1 - A;

    return 0;
}
