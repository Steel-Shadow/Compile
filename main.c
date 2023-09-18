#include <stdio.h>
int global_var = 0;

int func() {
    global_var = global_var + 1;
    return 1;
}

int main() {
    if (0 && func()) {
        ;
    }

    printf("%d", global_var);  // 输出 0

    if (1 || func()) {
        ;
    }

    printf("%d", global_var);  // 输出 0

    printf("21371421\n");
    return 0;
}