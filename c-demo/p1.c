#include <stdio.h>
#include <string.h>
int main() {
    int a, b;
    double c;
    scanf("%d%d%lf", &a, &b, &c);
    int num;
    double sum = 0;
    double price;
    double i;
    while (scanf("%lf%d", &price, &num) != EOF) {
        i += num * price;
    }
    int d = (int)(i / a);

    printf("%.2f", sum);
    return 0;
}
