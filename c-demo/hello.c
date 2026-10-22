#include <stdio.h>

int main(void) {
    int sum = 0;

    for (int i = 1; i <= 10; i++) {
        sum += i;
        printf("i = %2d, sum = %3d\n", i, sum);
    }

    printf("\n1 + 2 + ... + 10 = %d\n", sum);
    return 0;
}
