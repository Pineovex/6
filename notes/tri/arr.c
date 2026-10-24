/* 三语对照 · 同一件事的第一种写法
 * 任务：读入 n 和 n 个整数，边读边放进"能自动扩容的数组"，
 *      最后输出 个数 / 总和 / 最大值 / 最小值 / 去重升序结果。
 *
 * 这个文件重点看三处：
 *   1. 容量 c 与长度 n 是**两个**变量 —— 这是所有动态数组的核心
 *   2. realloc 之后旧指针可能失效，必须接住返回值
 *   3. 用完必须 free，否则内存泄漏（C 里没有 GC）
 *
 * 编译：gcc -g -Wall -Wextra -static -o arr.exe arr.c
 */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    if (scanf("%d", &n) != 1 || n <= 0) return 0;

    int *data = NULL;
    int len = 0;        /* 实际用了几个格子 */
    int cap = 0;        /* 一共申请了几个格子 */

    for (int i = 0; i < n; ++i) {
        int v;
        if (scanf("%d", &v) != 1) break;

        if (len == cap) {                 /* 满了 -> 扩容 */
            int newcap = (cap == 0) ? 4 : cap * 2;
            int *tmp = (int *)realloc(data, (size_t)newcap * sizeof(int));
            if (tmp == NULL) {            /* 扩容失败要收尾，不能直接 return */
                free(data);
                fprintf(stderr, "out of memory\n");
                return 1;
            }
            data = tmp;                   /* 关键：接住 realloc 的返回值 */
            cap = newcap;
        }
        data[len++] = v;                  /* 先放值，再让 len 加一 */
    }

    long long sum = 0;
    int mx = data[0], mn = data[0];
    for (int i = 0; i < len; ++i) {
        sum += data[i];
        if (data[i] > mx) mx = data[i];
        if (data[i] < mn) mn = data[i];
    }

    /* 去重升序：先排序，再跳过相邻重复 */
    for (int i = 1; i < len; ++i) {        /* 手写插入排序，不调 qsort 是为了看得清 */
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && data[j] > key) {
            data[j + 1] = data[j];
            --j;
        }
        data[j + 1] = key;
    }

    printf("count = %d\n", len);
    printf("sum   = %lld\n", sum);
    printf("max   = %d\n", mx);
    printf("min   = %d\n", mn);
    printf("uniq  =");
    for (int i = 0; i < len; ++i) {
        if (i > 0 && data[i] == data[i - 1]) continue;
        printf(" %d", data[i]);
    }
    printf("\n");

    free(data);                           /* 别忘了 */
    return 0;
}
