/*
 * PAT (Basic Level) 1003 我要通过！
 *
 * 判据（自己推一遍，别背结论）：
 *   规则2：xPATx 正确（x 是空串或全 A）
 *   规则3：aPbTc 正确 => aPbATca 也正确
 * 把 a、b、c 看成 P 前、P 与 T 之间、T 之后的 A 的个数：
 *   规则2 给出  a * 1 == c
 *   规则3 在 a*b == c 成立时，把 b 变成 b+1、c 变成 c+a，
 *   而 a*(b+1) == a*b + a == c + a，等式仍然成立。
 * 所以不变量是：  left * mid == right  且 mid >= 1，
 * 另外整串只能有 P、A、T，且 P、T 各一个、P 在 T 前面。
 */
#include <stdio.h>
#include <string.h>

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    char s[110];
    for (int i = 0; i < n; ++i) {
        if (scanf("%109s", s) != 1) break;

        int len = (int)strlen(s);
        int cntP = 0, cntT = 0, posP = -1, posT = -1;
        int bad = 0;                      /* 出现了 P/A/T 以外的字符 */

        for (int j = 0; j < len; ++j) {
            if (s[j] == 'P') { ++cntP; posP = j; }
            else if (s[j] == 'T') { ++cntT; posT = j; }
            else if (s[j] != 'A') { bad = 1; break; }
        }

        int ok = 0;
        if (!bad && cntP == 1 && cntT == 1 && posP < posT) {
            int left  = posP;             /* P 前面 A 的个数 */
            int mid   = posT - posP - 1;  /* P 和 T 之间 A 的个数 */
            int right = len - posT - 1;   /* T 后面 A 的个数 */
            if (mid >= 1 && left * mid == right) ok = 1;
        }

        puts(ok ? "YES" : "NO");
    }
    return 0;
}
